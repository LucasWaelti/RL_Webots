# pragma once

// Simulation parameters
#define TIMEOUT             1*30    // Seconds before starting a new rollout 
#define QUERY_PERIOD        0.01    // Period in seconds between each run of the step method (min is 0.008) 
#define WAIT_STEPS          floor(QUERY_PERIOD/(double)ROBOT_BASIC_TIMESTEP*1000.0)
#define ROLLOUTS            40000   // Number of rollouts to execute in total. 
#define ROLL_PER_SESSION    100     // Restart the simulation after 100 rollouts. 

// Training parameters
#define GAMMA               0.99    // Define the discount factor
#define BATCH_SIZE          500     // Batch size to train the network 
#define USE_VALUE_FUNCTION  true    // Whether to use a Value Function network to support the policy 
#define VALUE_UPDATE_RATE   20      // Number of rollouts before updating the weights of the value function

// Model saving/loading file
#define MODEL_PATH          "model/"
#define MODEL_POLICY_FILE   "model/vpg_policy"
#define MODEL_VALUE_FILE    "model/vpg_value" 
#define OPTIM_PATH          "optim/"
#define OPTIM_POLICY_FILE   "optim/optim_policy"
#define OPTIM_VALUE_FILE    "optim/optim_value"

// Data logging destination
#define LOG_PATH            "log/"
#define LOG_FILE            "log/log.csv"