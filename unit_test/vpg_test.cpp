#include <math.h>
#include <iostream>
#include <vector>
#include <vpg_wrapper.hpp>
#include <global_parameters.hpp>
#include <vpg_parameters.hpp>

static const std::string passed = "[\033[1;4;32mPASSED\033[0m]\n";
static const std::string failed = "[\033[1;4;31mFAILED\033[0m]\n";
static const std::string exception = "\n\033[1;33mFollowing exception occured:\033[0m\n";

void store_some_episodes(){
    VPG_episode e0;
    VPG_episode e1;
    VPG_episode e2;

    e0.action = {0,0};
    e1.action = {0.5,-0.5};
    e2.action = {1,1};

    e0.reward = 0.01;
    e1.reward = 0.02;
    e2.reward = 0.03;

    std::vector<double> state0(STATE_DIM, 1.);
    std::vector<double> state1(STATE_DIM, 0.);
    std::vector<double> state2(STATE_DIM, 0.5);
    e0.state = state0;
    e1.state = state1;
    e2.state = state2;
    e0.state_next = state2;
    e1.state_next = state1;
    e2.state_next = state0;

    // Push the episodes "randomly"
    vpg_w::store_episode(e0);
    vpg_w::store_episode(e1);
    vpg_w::store_episode(e2);
}

void test_memory_handling(){
    store_some_episodes();
    std::cout << "Memory storing test: " + (vpg_w::get_memory_size() != 3 ? failed : passed);
}

void test_reward_to_go(){
    bool success = true;
    double tol = 1e-10;
    vpg_w::reset_memory();
    store_some_episodes();
    
    // Test the reward_to_go computation
    vpg_w::compute_reward_to_go();
    const std::vector<VPG_episode> memory = vpg_w::get_memory();
    
    //std::cout << memory[0].reward_to_go << " " << memory[1].reward_to_go << " " << memory[2].reward_to_go << " "  << std::endl;
    std::cout << "Reward to go test: " + (success &&
        (abs(memory[0].reward_to_go - 0.01 - GAMMA*0.02 - GAMMA*GAMMA*0.03)>tol  || 
         abs(memory[1].reward_to_go - 0.02 - GAMMA*0.03)>tol || 
         abs(memory[2].reward_to_go - 0.03)>tol) ? failed : passed);
}

void test_eval_policy(){
    std::cout << "Policy evaluation test: " << std::flush;
    std::vector<float> p_actions;
    bool success = true;

    // Create a dummy state
    std::vector<double> v(STATE_DIM,0.);
    v.at(3) = 1.;
    v.at(2) = 1.;

    // Run a forward pass
    try{
        p_actions = vpg_w::eval_policy(v);
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }

    std::cout << (success ? passed : failed);
}

void test_value_function(){
    std::cout << "Value function test: " << std::flush;
    bool success = true;
    float value = 0.1234;
    std::vector<double> state(STATE_DIM,0.);
    state[0] = 1.;
    state[1] = 1.;

    try{
        value = vpg_w::eval_value_function(state);
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }
    if(value == 0.1234) success = false;

    std::cout << (success ? passed : failed);
}

void test_model_saving(std::string path, std::string file){
    std::cout << "Saving test: " << std::flush; 
    int success = true;
    try{
        vpg_w::save(path,file,POLICY);
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }
    std::cout << (success ? passed : failed);
}

void test_model_loading(std::string file){
    std::cout << "Loading test: " << std::flush; 
    int success = true;
    try{
        vpg_w::load(file,POLICY);
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }
    std::cout << (success ? passed : failed);
}

void test_training(){
    bool success = true;

    // Empty the memory
    vpg_w::reset_memory();
    // Push some new episodes
    store_some_episodes();

    // Run a training sequence
    try{
        vpg_w::train();
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }
    std::cout << "Training test: " + (success ? passed : failed);
}

void test_training_heavy(){
    std::cout << "Heavy training test: " << std::flush;
    bool success = true;

    // Empty the memory
    vpg_w::reset_memory();
    // Prepare a dummy episode
    VPG_episode e0;
    e0.action = {0,0};
    e0.reward = 0.01;
    std::vector<double> state0(STATE_DIM, 1.);
    e0.state = state0;
    e0.state_next = state0;
    // Store it N times
    for(int i=0; i<1000; i++){
        vpg_w::store_episode(e0);
    }
    vpg_w::compute_reward_to_go();

    // Run a training sequence
    try{
        vpg_w::train(100);
    }catch(const std::exception& e){
        success = false;
        std::cout << exception << e.what() << std::endl;
    }
    std::cout << (success ? passed : failed);
}

int main(int argc, char* argv[]){

    // Initilize the network
    vpg_w::init();

    // Test the memory for episodes handling 
    test_memory_handling();

    test_reward_to_go();

    // Test the proba generation
    test_eval_policy();

    // Test value function evaluation
    test_value_function();

    // Test loading and saving the model
    system("rm -r model");
    test_model_loading("model/vpg_policy");
    test_model_saving("model/","model/vpg_policy");
    test_model_loading("model/vpg_policy");

    // Test vector constructor's properties
    /*std::vector<int> v1 = {0,1,2,3,4};
    std::vector<int> v2 = std::vector<int>(v1.begin(), v1.begin()+v1.size());
    std::cout << "v1 == v2 ? " << (v1==v2) << std::endl; // true
    for(int i=0; i<v2.size(); i++)
        std::cout << v2[i];
    std::cout << std::endl;*/

    // Test the training procedure
    test_training();

    // Test the training procedure with lots of epsiodes
    test_training_heavy();

    return 0;
}
