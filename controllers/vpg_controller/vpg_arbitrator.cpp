#include "vpg_arbitrator.hpp"

VPG_arbitrator::VPG_arbitrator(){

    // Robot Initialization
    k4::init_robot(); 

    // Initialize the time stamp
    this->m_timestamp = 0; 

    // Initilaize the current action
    this->m_current_action = {0.0,0.0};
    
    // Set invalide time to episodes
    this->m_current_episode.t = -1; 
    this->m_previous_episode.t = -1;

    // Default mode is training 
    this->m_is_training = true;
    this->m_verbose_eval = false;

    // Init the reward
    this->m_reward = Reward();

    // Initialize the wrapper for the network
    vpg_w::init(); 

    // Create the log file if not existing 
    create_log(LOG_PATH, LOG_FILE, log_columns);

    // Initialize the rollout count
    std::cout << "Reading the log. " << std::endl;
    this->m_rollout_count = read_last_rollout(LOG_FILE);
    this->m_rollout_local = 0;
}

VPG_arbitrator::~VPG_arbitrator(){}

int VPG_arbitrator::idle(int steps){
    int status = 1;
    for(int i=0; i<steps && status; i++){
        k4::set_normalized_motors_speed(this->m_current_action[0]*2.0-1.0,
                                        this->m_current_action[1]*2.0-1.0);
        status = k4::khepera_running();
    }
    return status;
}

int VPG_arbitrator::_training_step(){

    // Handle number of rollouts
    if(this->m_rollout_count >= ROLLOUTS){
        // All rollouts executed, stop controller
        std::cout << "All rollouts executed. Terminating." << std::endl;
        return 0;
    }
    
    // Check if time out is met or if collision occured
    bool timed_out = (TIMEOUT <= this->get_timestamp()*WAIT_STEPS*((double)ROBOT_BASIC_TIMESTEP)*0.001);
    bool collided = detect_collision();

    if(timed_out || collided){

        std::cout << "Time stamp: " << m_timestamp << ", duration in seconds: " 
            << (double)m_timestamp*WAIT_STEPS*(double)ROBOT_BASIC_TIMESTEP/1000.0 << std::endl;

        // Store the episode with the final reward
        this->build_final_episode();
        vpg_w::store_episode(this->m_previous_episode);

        this->m_rollout_count++;
        this->m_rollout_local++;

        std::cout << "Terminated rollout [" << this->m_rollout_count << "/" << ROLLOUTS << "]"  << std::endl;
        vpg_w::compute_reward_to_go();
        this->train();
        
        vpg_w::save(MODEL_PATH,MODEL_POLICY_FILE,POLICY);
        vpg_w::save(MODEL_PATH,MODEL_VALUE_FILE, VALUE);
        write_line(LOG_FILE, VPG_arbitrator::gather_log_data());
        
        this->reset();

        supervisor_move_robot();
        wb_supervisor_simulation_reset_physics(); 
    }

    // Increment timestamp for next call 
    this->m_timestamp++;
    
    // Exploit policy
    this->m_current_action = vpg_w::eval_policy(this->m_last_states.get_state(CURRENT_STATE));
    //std::cout << "New state encountered, trying action: " << this->m_current_action << std::endl; 

    // Apply action (make sure it is properly applied)
    k4::set_normalized_motors_speed(this->m_current_action[0]*2.0-1.0,
                                    this->m_current_action[1]*2.0-1.0);
    
    // Build episode 
    this->build_episodes(this->m_current_action); 

    // Send the previous_state if valid to the network implementation for storing
    if(this->m_previous_episode.t > -1){
        vpg_w::store_episode(m_previous_episode);
    }

    // Roll episodes for next step
    this->roll_episodes();

    // Add to travelled distance
    measure_travelled_distance(this->m_current_action[0]*2.0-1.0,
                                this->m_current_action[1]*2.0-1.0);

    return k4::khepera_running(); 
}

int VPG_arbitrator::_eval_step(){
    // Increment timestamp for next call 
    this->m_timestamp++;

    // Run inference on the network 
    this->m_current_action = vpg_w::eval_policy(m_last_states.get_state(CURRENT_STATE));

    // Print the action probabilities and the selected action
    if(this->m_verbose_eval){
        std::cout << std::fixed;
        std::cout << std::setprecision(2);
        std::cout << "State: " << this->m_last_states.to_string(CURRENT_STATE) << std::endl;
        std::cout << "Action: ";
        for(uint i=0; i<ACTION_DIM; i++){
            std::cout << this->m_current_action[i] << ",";
        }
        if(USE_VALUE_FUNCTION)
            std::cout << " - state value: " << vpg_w::eval_value_function(m_last_states.get_state(CURRENT_STATE));
        std::cout << std::endl;
    } 

    // Apply action
    k4::set_normalized_motors_speed(this->m_current_action[0]*2.0-1.0, 
                                    this->m_current_action[1]*2.0-1.0);

    return k4::khepera_running();
}

int VPG_arbitrator::step(){

    // Set the query period 
    for(int i=0; i<WAIT_STEPS; i++){
        if(!k4::khepera_running())
            return 0;
    }

    // Observe current state
    this->m_last_states.step();
    
    // Get current reward
    this->m_reward.compute_reward(this->m_last_states.get_states(), this->m_timestamp);
    
    if(this->m_is_training)
        return this->_training_step();
    else
        return this->_eval_step(); 
}

void VPG_arbitrator::reset(){
    // Initialize the time stamp
    this->m_timestamp = 0; 
    // Initialize the current action 
    this->m_current_action = {0.0,0.0};
    // Set invalide time to episodes
    this->m_current_episode.t = -1;
    this->m_previous_episode.t = -1;

    this->m_last_states.reset();

    this->m_reward.reset();

    reset_distance(); 

    vpg_w::reset_memory();
    vpg_w::reset_max_loss(); 
}

void VPG_arbitrator::build_episodes(std::vector<float> action){
    this->m_current_episode.t = this->m_timestamp;
    this->m_current_episode.state = this->m_last_states.get_state(CURRENT_STATE);
    std::vector<double> to_double(action.begin(), action.end());
    this->m_current_episode.action = to_double; 
    this->m_previous_episode.reward = this->m_reward.get_reward();
    this->m_previous_episode.state_next = this->m_last_states.get_state(CURRENT_STATE);
}

void VPG_arbitrator::build_final_episode(){
    this->m_previous_episode.reward = this->m_reward.get_reward();
    this->m_previous_episode.state_next = this->m_last_states.get_state(CURRENT_STATE);
}

void VPG_arbitrator::roll_episodes(){
    this->m_previous_episode.t = this->m_timestamp;
    this->m_previous_episode.state = this->m_last_states.get_state(CURRENT_STATE);
    this->m_previous_episode.action = this->m_current_episode.action; 
}

void VPG_arbitrator::mode(bool evaluating, bool verbose){
    this->m_is_training = !evaluating;
    this->m_verbose_eval = verbose;
}

void VPG_arbitrator::train(){
    vpg_w::train();
}

int VPG_arbitrator::get_timestamp(){
    return this->m_timestamp;
}

int VPG_arbitrator::get_rollout_count(){
    return this->m_rollout_count;
}

bool VPG_arbitrator::is_training(){
    return this->m_is_training;
}

// {"Rollout", "Num_episodes", "Num_timesteps", "Cumulated_reward", "Max_loss", "Distance"}

std::vector<std::string> VPG_arbitrator::gather_log_data(){
    std::vector<std::string> line;
    // Get the rollout info
    line.push_back(std::to_string(this->m_rollout_count));
    // Get the number of used episodes
    line.push_back(std::to_string(vpg_w::get_memory_size()));
    // Get the number of timesteps
    line.push_back(std::to_string(this->m_timestamp));
    // Get the cumulated reward
    line.push_back(std::to_string(this->m_reward.get_cumulated_reward()));
    // Get the maximum loss during rollout
    line.push_back(std::to_string(vpg_w::get_max_loss()));
    // Get the travelled distance 
    line.push_back(std::to_string(get_distance()));
    return line;
}
