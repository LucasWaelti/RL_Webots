#include "vpg_episode.hpp"

std::string VPG_episode::to_string() const{
    std::string output = "Episode " + std::to_string(this->t) + ":\n\tstate: ";
    for(unsigned int i=0; i<this->state.size(); i++){
        output += std::to_string(this->state[i]) + " ";
    }
    output += ",\n\taction: " + std::to_string(this->action[0]) + ", " + std::to_string(this->action[1]) + ",\n";
    output += "\treward: " + std::to_string(this->reward) + ",\n";
    output += "\treward to go: " + std::to_string(this->reward_to_go) + ",\n\tnext state: ";
    for(unsigned int i=0; i<this->state_next.size(); i++){
        output += std::to_string(this->state_next[i]) + " ";
    }
    return output;
}
