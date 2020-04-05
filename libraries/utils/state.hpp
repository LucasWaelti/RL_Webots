#pragma once 

#include <iostream>
#include <vector>
#include <memory> 
#include <math.h>

// Kephera API
#include <khepera.hpp>

#include <global_parameters.hpp>

#define PREV_STATE      0
#define CURRENT_STATE   1

typedef std::vector<std::vector<double>> t_state; 

/**
 * The State class stores the two last observed states
 * as 1D vectors. 8 IR values, 2 wheel speeds. 
 */
class State{
    public:
        State();

        /**
         * Make state measurment. 
         */
        void measure();

        /**
         * Convert the state to a string. 
         * t = 0 (PREV_STATE) is previous, = 1 (CURRENT_STATE) is last measured
         */
        std::string to_string(uint t = CURRENT_STATE);

        /**
         * t = 0 (PREV_STATE) is previous, = 1 (CURRENT_STATE) is last measured
         */
        std::vector<double> get_state(uint t = CURRENT_STATE);
        t_state get_states();
        t_state step();

        void reset();

    protected: 

    private: 
        t_state m_states;
};
