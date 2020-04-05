#pragma once 
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <random>
#include <vector>

#include "vpg_episode.hpp" 

#include <vpg_parameters.hpp>
#include <global_parameters.hpp>

// Generator's seed
#define SEED 12345 


/**
 * Namespace "vpg" containing the Vanilla Policy Gradient implementation. 
 */
namespace vpg{

    // Random generator
    std::default_random_engine generator;
    // Seeds the generator for reproducibility 
    void seed_generator();
    
    // --------------------- Policy Network ---------------------
    struct PolicyImpl: torch::nn::Module{
        PolicyImpl();
        torch::Tensor forward(torch::Tensor x);
        torch::nn::Linear l1{nullptr},l2{nullptr},l3{nullptr};
    };
    TORCH_MODULE(Policy);
    // Network instance
    static Policy policy_model; 
    // Optimizer used to train the network
    static torch::optim::Adam* p_optimizer;

    // --------------------- Value function Network ---------------------
    struct ValueImpl: torch::nn::Module{
        ValueImpl();
        torch::Tensor forward(torch::Tensor x);
        torch::nn::Linear l1{nullptr},l2{nullptr},l3{nullptr};
    };
    TORCH_MODULE(Value);
    // Network instance
    static Value value_model; 
    // Optimizer used to train the network
    static torch::optim::Adam* v_optimizer; 

    // Rollout counter 
    static int value_rollout_count = 0;
    // -----------------------------------------------------------------


    // Vector storing the generated episodes 
    static std::vector<VPG_episode> memory;
    static std::vector<VPG_episode> memory_batch;

    /**
     * Extract the states stored by the episodes inside the memory. 
     * @param next (bool) If false, the state of the episode is queried. 
     *              Otherwise the next state of the episode is returned. 
     */
    std::vector<std::vector<double>> get_state_from_memory_batch(bool next = false);

    torch::Tensor get_action_from_memory_batch();

    torch::Tensor get_reward_from_memory_batch();

    torch::Tensor get_reward_to_go_from_memory_batch();

    /**
     * Utility method to convert a state vector into a Tensor including a dimension for the batch. 
     * @param state (std::vector<double>) 
     */
    torch::Tensor convert_state_to_batch(std::vector<double> state);

    /**
     * Utility method to convert a vector of state vectors into a Tensor generating an actual batch.
     * @overload convert_state_to_batch(std::vector<double> state)
     * @param states (std::vector<std::vector<double>>) 
     */
    torch::Tensor convert_state_to_batch(std::vector<std::vector<double>> states);

    /**
     * Compute parameters gradients based on the current memory state when called. 
     */
    int compute_gradients();

    // Maximum loss encountered during a rollout 
    static float max_loss = 0.0F;

    /**
     * Update the maximum loss. 
     */
    void update_max_loss(float loss);
}

