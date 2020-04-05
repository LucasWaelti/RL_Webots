#include "reward.hpp"

/**
 * Verify whether one if the IR sensor is seeing an 
 * obstacle. 
 */
bool _obstacle_present(const std::vector<double>& a){
    for(int i=0; i<STATE_NUM_IR; i++){
        if(a[i] > 0.0)
            return true; 
    }
    return false; 
}

Reward::Reward(){
    reset(); 
}

double Reward::compute_reward(t_state state, int timestamp){

    m_reward = 0.; 

    // Adapt speed representation
    state[CURRENT_STATE][STATE_LEFT]  = state[CURRENT_STATE][STATE_LEFT] *2.0-1.0; 
    state[CURRENT_STATE][STATE_RIGHT] = state[CURRENT_STATE][STATE_RIGHT]*2.0-1.0; 

    // Reward driving forward when no obstacle
    if(!_obstacle_present(state.at(CURRENT_STATE))){
        if(state.at(CURRENT_STATE)[STATE_LEFT] > 0.0
            && state.at(CURRENT_STATE)[STATE_RIGHT] > 0.0){
            m_reward += 0.01;
            if(abs(state.at(CURRENT_STATE)[STATE_LEFT] - state.at(CURRENT_STATE)[STATE_RIGHT]) < 0.1)
                m_reward += 0.01;
            else 
                m_reward -= 0.02;
        }
        else 
            m_reward -= 0.01; 
    }

    // Reward clearing obstacles
    if(_obstacle_present(state.at(PREV_STATE)) && 
        !_obstacle_present(state.at(CURRENT_STATE)))
        m_reward += 1.0; 

    // Penalize collisions (override reward)
    if(detect_collision())
        m_reward = -1.0; 
    
    m_cumulated_reward += m_reward;

    return m_reward;
}

double Reward::get_reward(){
    return m_reward;
}

double Reward::get_cumulated_reward(){
    return m_cumulated_reward;
}

void Reward::reset(){
    m_reward = 0.;
    m_cumulated_reward = 0.;
}