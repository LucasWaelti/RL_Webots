#pragma once 

#include <iostream>
#include <string>
#include <vector>

/**
* Structure storing an Episode 
*   {state, action, reward, reward_to_go, state_next}
* At time t, `state` and `action` are sampled. 
* At time t+1, `reward` and `state_next` are sampled. 
* Once a rollout is complete, the reward_to_go can be computed. 
*/
struct VPG_episode{
    int t;                          // Timestamp. 
    std::vector<double> action;     // Action taken by the agent when in `state`. 
    double reward;                  // Reward perceived by the agent for performing `action` in `state`. 
    double reward_to_go;            // Cumulated reward evaluated from time t. 
    std::vector<double> state;      // State when taking the action. 
    std::vector<double> state_next; // State after taking the action. 

    /**
     * Convert Episode to string:
     * "Episode x:
     *      state: x x x x x x 
     *      action: x
     *      reward: x 
     *      reward to go: x
     *      next state: x x x x x x 
     * "
     */
    std::string to_string() const;

} typedef DQN_episode;