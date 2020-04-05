#include "vpg_network.hpp"
#include "vpg_wrapper.hpp"

/////////////////////////////////////////////////////////////////////
// WRAPPER METHODS IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void vpg_w::init(){

    //vpg::seed_generator();

    // Instantiate the models
    if(!vpg_w::load(MODEL_POLICY_FILE,POLICY)){
        vpg::policy_model = vpg::Policy();
    }
    if(!vpg_w::load(MODEL_VALUE_FILE,VALUE)){
        vpg::value_model = vpg::Value();
    }

    // Set the uninitialised policy optimizer pointer
    static torch::optim::Adam p_adam(vpg::policy_model->parameters(), 
        torch::optim::AdamOptions(1e-3)); // .beta1(0.5)
    vpg::p_optimizer = &p_adam;

    // Set the uninitialised value function optimizer pointer
    static torch::optim::Adam v_adam(vpg::value_model->parameters(), 
        torch::optim::AdamOptions(1e-4)); // .beta1(0.5)
    vpg::v_optimizer = &v_adam;

    // Set the maximum loss to zero
    vpg_w::reset_max_loss();
}

void vpg_w::store_episode(VPG_episode episode){
    vpg::memory.push_back(episode);
}

void vpg_w::compute_reward_to_go(){
    double rtg = 0.;
    if(vpg::memory.empty())
        return;
    for(uint i=vpg::memory.size()-1; i>=0; i--){
        rtg = vpg::memory[i].reward + GAMMA*rtg;
        vpg::memory[i].reward_to_go = rtg;
        if(i==0) break;
    }
}

uint vpg_w::get_memory_size(){
    return vpg::memory.size();
}

const std::vector<VPG_episode>& vpg_w::get_memory(){
    return vpg::memory;
}

void vpg_w::reset_memory(){
    vpg::memory.clear();
}

std::vector<float> vpg_w::eval_policy(std::vector<double> state){
    torch::Tensor proba = vpg::policy_model->forward(
                torch::_cast_Float(vpg::convert_state_to_batch(state)));
    std::vector<float> action(ACTION_DIM, 0.0F);
    for(uint i=0; i<ACTION_DIM; i++){
        action.at(i) = proba[0][i].item().toFloat();
    }
    return action;
}

float vpg_w::eval_value_function(std::vector<double> state){
    return vpg::value_model->forward(torch::_cast_Float(vpg::convert_state_to_batch(state))).item().toFloat();
}

void vpg_w::train(uint batch_size){
    if(vpg::memory.size() == 0){
        std::cout << "\033[1;33m[WARNING] memory empty, training aborted.\033[0m" << std::endl; 
        return;
    }

    // Clear the gradients of the models' parameters
    vpg::policy_model->zero_grad();
    vpg::value_rollout_count++; 
    if(vpg::value_rollout_count > VALUE_UPDATE_RATE){
        vpg::value_rollout_count = 0;
        vpg::value_model->zero_grad();
    }

    // Split the memory into batches and compute the gradients on each batch sequentially 
    for(uint i=0 ;; i++){
        //std::cout << "Processing batch: " << i << std::endl;
        if(batch_size >= vpg::memory.size()){ // Batch size is at least as large as the memory
            vpg::memory_batch = std::vector<VPG_episode>(vpg::memory.begin(), vpg::memory.end());
            vpg::compute_gradients();
            break;
        }else if((i+1)*batch_size <= vpg::memory.size()){ // Extract a batch from the memory
            vpg::memory_batch = std::vector<VPG_episode>(vpg::memory.begin() + i*batch_size, vpg::memory.begin() + (i+1)*batch_size);
            vpg::compute_gradients();
        }else if(i*batch_size < vpg::memory.size() && (i+1)*batch_size >= vpg::memory.size()){ // If the last batch overshoots the memory's end
            vpg::memory_batch = std::vector<VPG_episode>(vpg::memory.begin() + i*batch_size, vpg::memory.end());
            vpg::compute_gradients();
            break;
        }else{ // Reached end of memory (i*batch_size == memory.size()) -> stop (was already handled in first "else if")
            //std::cout << "\033[1;33mConditional Miss\033[0m, processing aborted. Status:" << std::endl;
            //std::cout << "Memory size: " << vpg::memory.size() << std::endl;
            //std::cout << "Start: " << i*batch_size << "\t End: " << (i+1)*batch_size << std::endl;
            break; 
        }
    }

    // Update the parameters
    vpg::p_optimizer->step();
    if(vpg::value_rollout_count == VALUE_UPDATE_RATE && USE_VALUE_FUNCTION){
        std::cout << "Cumulated gradients for " << 
            VALUE_UPDATE_RATE << " rollouts. Updating value function weights." << std::endl;
        vpg::v_optimizer->step();
    }
}

float vpg_w::get_max_loss(){
    return vpg::max_loss;
}

void vpg_w::reset_max_loss(){
    vpg::max_loss = 0.0F;
}

bool vpg_w::_dir_exists(std::string path){
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
}

void vpg_w::_create_path(std::string path){
    const std::string command = "mkdir -p " + path;
    system(command.c_str());
}

void vpg_w::save(std::string path, std::string file, uint model){
    if(!_dir_exists(path))
        _create_path(path);
    
    switch(model){
        case POLICY: 
            torch::save(vpg::policy_model,file); 
            break;
        case VALUE:
            torch::save(vpg::value_model,file); 
            break;
        /*case POLICY_OPTIM:
            torch::save(vpg::p_optimizer,file);
            break;
        case VALUE_OPTIM:
            torch::save(vpg::v_optimizer,file);
            break;*/
    }
}

bool vpg_w::load(std::string file, uint model){
    // Load the model
    try{
        if(model == POLICY){
            torch::load(vpg::policy_model,file); 
            return true;
        }else if(model == VALUE){
            torch::load(vpg::value_model,file); 
            return true;
        }
    }catch(const std::exception& e){
        std::cout << "\n\033[1;33m[WARNING]:\033[0m Could not load specified model. Using untrained version.\n";
        return false;
    }
}

/////////////////////////////////////////////////////////////////////
// CORE METHODS IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void vpg::seed_generator(){
    vpg::generator.seed(SEED);
}

vpg::PolicyImpl::PolicyImpl(){
    // Constructor - build the network's layers
    this->l1 = register_module("l1",torch::nn::Linear(STATE_DIM,8)); 
    this->l2 = register_module("l2",torch::nn::Linear(8,4));
    this->l3 = register_module("l3",torch::nn::Linear(4,ACTION_DIM));
}

torch::Tensor vpg::PolicyImpl::forward(torch::Tensor x){
    // apply ReLU activations and sigmoid on output
    x = torch::relu(this->l1->forward(x));
    x = torch::relu(this->l2->forward(x));
    x = torch::sigmoid(this->l3->forward(x)); 

    return x;
}

vpg::ValueImpl::ValueImpl(){
    // Constructor - build the network's layers
    this->l1 = register_module("l1",torch::nn::Linear(STATE_DIM,5));
    this->l2 = register_module("l2",torch::nn::Linear(5,3));
    this->l3 = register_module("l3",torch::nn::Linear(3,1));
}

torch::Tensor vpg::ValueImpl::forward(torch::Tensor x){
    // apply ReLU activations and leave the output linear
    x = torch::relu(this->l1->forward(x));
    x = torch::relu(this->l2->forward(x));
    x = this->l3->forward(x); 

    return x;
}

std::vector<std::vector<double>> vpg::get_state_from_memory_batch(bool next){
    std::vector<std::vector<double>> states;
    for(uint i=0; i<vpg::memory_batch.size(); i++){
        if(!next){
            states.push_back(vpg::memory_batch[i].state);
        }else{
            states.push_back(vpg::memory_batch[i].state_next);
        }
    }
    return states;
}

torch::Tensor vpg::get_action_from_memory_batch(){
    std::vector<double> actions;
    for(uint i=0; i<vpg::memory_batch.size(); i++){
        actions.push_back(vpg::memory_batch[i].action[0]);
        actions.push_back(vpg::memory_batch[i].action[1]);
    }
    return torch::_cast_Float(torch::tensor(actions).view({-1,2}));
}

torch::Tensor vpg::get_reward_from_memory_batch(){
    std::vector<double> rewards;
    for(uint i=0; i<vpg::memory_batch.size(); i++){
        rewards.push_back(vpg::memory_batch[i].reward);
    }
    return torch::_cast_Float(torch::tensor(rewards).view({-1,1}));
}

torch::Tensor vpg::get_reward_to_go_from_memory_batch(){
    std::vector<double> rtg;
    for(uint i=0; i<vpg::memory_batch.size(); i++){
        rtg.push_back(vpg::memory_batch[i].reward_to_go);
    }
    return torch::_cast_Float(torch::tensor(rtg).view({-1,1}));
}

torch::Tensor vpg::convert_state_to_batch(std::vector<double> state){
    return torch::tensor(state).view({-1,(long)state.size()});
}
torch::Tensor vpg::convert_state_to_batch(std::vector<std::vector<double>> states){
    torch::Tensor t = vpg::convert_state_to_batch(states[0]);
    for(uint i=1; i<states.size(); i++){
        t = torch::cat({t,vpg::convert_state_to_batch(states[i])}, /*dim=*/0);
    }
    return t;
}

bool _has_nan(torch::Tensor t){
    torch::Tensor b = at::isnan(t);
    return at::any(b).item().toBool();
    c10::IntArrayRef sizes = b.sizes(); 

    for(int i=0; i<sizes[0]; i++){
        for(int j=0; j<sizes[1]; j++){
            if(b[i][j].item().toBool())
                return true; 
        }
    }
    return false; 
}

int vpg::compute_gradients(){
    // Get the states (network's input)
    torch::Tensor states = torch::_cast_Float(vpg::convert_state_to_batch(vpg::get_state_from_memory_batch()));
    // Get the next states that were the result of the chosen action
    torch::Tensor next_states = torch::_cast_Float(vpg::convert_state_to_batch(vpg::get_state_from_memory_batch(true)));
    // Get the actions (network's output)
    torch::Tensor actions = vpg::policy_model->forward(states);
    // Get the actually taken actions
    //torch::Tensor actions = torch::_cast_Int(vpg::get_action_from_memory_batch());
    // Get the rewards
    torch::Tensor rewards = vpg::get_reward_from_memory_batch();
    // Get the rewards to go
    torch::Tensor rewards_to_go = vpg::get_reward_to_go_from_memory_batch();

    ////////////////////////////////////////////////////////////////////////////
    // Train the Policy
    ////////////////////////////////////////////////////////////////////////////

    // Compute the advantage
    torch::Tensor advantage, s_values;
    if(USE_VALUE_FUNCTION){
        // Get the states values
        s_values = vpg::value_model->forward(states);
        advantage = rewards_to_go - s_values; 
    }else{
        // Compute the advantage without support from the value function
        advantage = rewards_to_go;
    }
    
    // Compute the loss (negative sign to perform gradient ascent) 
    torch::Tensor policy_loss = -torch::log(actions) * advantage; 

    // Compute the backward pass on each component of the loss {N,1}
    for(uint i=0; i<policy_loss.size(0); i++){
        policy_loss[i][0].backward(/*gradient*/{}, /*keep_graph*/true);
        policy_loss[i][1].backward(/*gradient*/{}, /*keep_graph*/true);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    // Train the Value Function
    ////////////////////////////////////////////////////////////////////////////
    if(USE_VALUE_FUNCTION){
        // Compute the Value function's loss based on the reward to go 
        torch::Tensor value_loss = torch::mse_loss(s_values, rewards_to_go, /*reduction*/1L);

        // Update the maximum loss
        vpg::update_max_loss(value_loss.max().item().toFloat());

        // Compute the gradients
        value_loss.backward();
    }
    return 0;
}

void vpg::update_max_loss(float loss){
    if(max_loss < loss)
        max_loss = loss;
}