#pragma once 

#include <vector>
#include <sstream>
#include <sys/stat.h>

#include "vpg_episode.hpp" 

enum SAVE_ARGS {POLICY,VALUE,POLICY_OPTIM,VALUE_OPTIM};

// Namespace for the functions of the VPG wrapper 
namespace vpg_w{

    /**
     * Initialize the networks and other parameters. 
     */
    void init();

    /**
     * Store provided episode to use later for training. 
     * @param episode (Episode) {state, action, reward, next state} tuple. 
     */
    void store_episode(VPG_episode episode);

    /**
     * Compute the reward_to_go of the currently stored episodes. 
     */
    void compute_reward_to_go();

    /**
     * @return (uint) number of episodes currently stored. 
     */
    uint get_memory_size();

    /**
     * @return (std::vector<VPG_episode>) get the memory's reference. 
     */
    const std::vector<VPG_episode>& get_memory(); 

    /**
     * Dump the currently stored episodes. 
     */
    void reset_memory();

    /**
     * Run inference on the policy network and return the action probabilities. 
     * @param state (std::vector<double>) Robot's state. 
     * @return (std::vector<float>) Each action's probability. 
     */
    std::vector<float> eval_policy(std::vector<double> state);

    /**
     * Run inference on the value function network and return the state's value. 
     * @param state (std::vector<double>) Robot's state. 
     * @return (float) State's estimated value.  
     */
    float eval_value_function(std::vector<double> state);

    /**
     * Train the network based on the currently stored episodes. 
     * @param batch_size (uint) Number of episodes to use in a single batch. 
     */
    void train(uint batch_size=100);

    /**
     * Get the maximum loss. 
     */
    float get_max_loss();
    
    /**
     * Reset the maximum loss. 
     */
    void reset_max_loss();

    /**
     * Check if the specified directory exists.
     * @param path (std::string)
     * @return (bool)
     */
    bool _dir_exists(std::string path);

    /**
     * Create the specified directory exists.
     * @param path (std::string)
     */
    void _create_path(std::string path);

    /**
     * Save the model into the specified file
     * @param path (std::string) path to the file
     * @param file (std::string) path to the file + file name
     * @param model (uint) whether to save the policy network (POLICY=0) or the value function network (VALUE=1)
     */
    void save(std::string path, std::string file, uint model);

    /**
     * Load the model from the specified file
     * @param file (std::string) path to the file + file name
     * @param model (uint) whether to load the policy network (POLICY=0) or the value function network (VALUE=1)
     * @return (bool) whether loading has succeeded. 
     */
    bool load(std::string file, uint model);
}
