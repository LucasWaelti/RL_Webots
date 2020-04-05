#pragma once 

// Standard library 
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <random>

// Utils
#include <khepera.hpp>
#include <state.hpp>
#include <reward.hpp>
#include <training_handler.hpp>

// Network 
#include <vpg_wrapper.hpp>  
#include "vpg_parameters.hpp"

#include <global_parameters.hpp> 

#include <data_logger.hpp>
// Log file column names
const std::vector<std::string> log_columns = {"Rollout", "Num_episodes", 
            "Num_timesteps", "Cumulated_reward", "Max_loss", "Distance"};

/**
 * VPG_arbitrator orchestrates the learning and evaluation of the network and
 * handles state and reward computations. 
 */
class VPG_arbitrator{
    public:
        /**
         * Instantiate the robot. 
         */
        VPG_arbitrator();

        /**
         * Cleanup webots resources. 
         */
        ~VPG_arbitrator();

        /**
         * Step the robot once to let the sensors settle (executes STOP behavior)
         * @param steps (int) number of steps to idle for. 
         */
        int idle(int steps=1);

        /**
         * Step the robot. Action depends on mode (train/eval). 
         */
        int step();

        /**
         * Reset the arbitrator. 
         */
        void reset();

        /**
         * Detect task completion
         * @return (bool) indicating wether task is complete or not. 
         */
        bool detect_task_completion();

        /**
         * Build the episodes that are required to train the network.
         * @param action (int) Action taken at time t.  
         */
        void build_episodes(std::vector<float> action);

        /**
         * Build the final epsiode containing the final reward.
         */
        void build_final_episode();

        /**
         * Transfer data between current and previous episodes for next iteration. 
         */
        void roll_episodes();

        /**
         * Set the arbitrator in evaluation or training mode. 
         * @param evaluating (bool) evaluation mode if true, training mode otherwise (default). 
         * @param verbose (bool) print the action probabilities after evaluating the state when in evaluation mode. 
         */
        void mode(bool evaluating = true, bool verbose = false);

        /**
         * Train the network on the precedent roll out.  
         */
        void train();

        /**
         * Get the current timestamp of the arbutrator. 
         */
        int get_timestamp();

        /**
         * Get the current rollout count. 
         */
        int get_rollout_count();

        /**
         * Tells if in training mode or eval mode. 
         * @return (bool) training
         */
        bool is_training();

        /**
         * Retreive all the stats relative to the rollout. 
         * @return (std::vector<std::string>) vector of values. 
         */
        std::vector<std::string> gather_log_data();
    
    protected:  

    private:
        int _training_step(); 
        int _eval_step();
        int m_timestamp;
        int m_rollout_count;
        int m_rollout_local;
        bool m_is_training; // default is true
        bool m_verbose_eval;
        std::vector<float> m_current_action; // Left and right wheels velocity 
        State m_last_states;
        Reward m_reward;
        VPG_episode m_current_episode;
        VPG_episode m_previous_episode; 
};