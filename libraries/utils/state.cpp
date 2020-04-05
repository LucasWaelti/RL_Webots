#include "state.hpp"

State::State(){

    // Set the default starting states (all false)
    std::vector<double> v(STATE_DIM,0.);
    this->m_states.push_back(v);
    this->m_states.push_back(v);
}

void State::measure(){
    // Roll states
    m_states[PREV_STATE] = m_states[CURRENT_STATE];

    // Get the sensor readings
    std::vector<double> state = k4::get_normalized_ranges();
    std::vector<double> speeds = k4::get_normalized_motors_speed();
    state.push_back(speeds[0]);
    state.push_back(speeds[1]);
    m_states[CURRENT_STATE] = state;
}

std::string State::to_string(uint t){
    std::string string = "";
    for(uint i=0; i<this->m_states[t].size(); i++){
        string += std::to_string(this->m_states[t][i]) + " ";
    }
    return string;
}

std::vector<double> State::get_state(uint t){
    return this->m_states.at(t);
}

t_state State::get_states(){
    return this->m_states;
}

t_state State::step(){
    this->measure();
    return this->get_states();
}

void State::reset(){
    // Set the default starting state (all 0)
    for(uint i=0; i<STATE_DIM; i++){
        this->m_states[0].at(i) = 0.;
        this->m_states[1].at(i) = 0.;
    }
}
