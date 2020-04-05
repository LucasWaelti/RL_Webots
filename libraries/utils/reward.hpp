#include <math.h>

#include "state.hpp"
#include "training_handler.hpp"

/**
 * The Reward class enables to compute the reward associtated
 * to a given State. It stores the current (last evaluated) reward, 
 * and the cumulated reward. 
 */
class Reward{
    public:
        /**
         * Reward constructor. 
         */
        Reward();

        /**
         * Compute the reward selected when instantiating the class. 
         * @param state (std::vector<double>) Robot's current state. 
         * @param timestamp (int) Rollout time stamp at which the reward is computed. If equal to 0, no decay is applied. 
         */ 
        double compute_reward(t_state state, int timestamp = 0);

        /**
         * Get the last computed reward. 
         * @output (double) reward. 
         */
        double get_reward();

        /**
         * Get the cumulated computed reward. 
         * @output (double) cumulated reward. 
         */
        double get_cumulated_reward();

        /**
         * Reset the currently stored values. 
         */
        void reset();

    private: 
        double m_reward;
        double m_cumulated_reward;
        double m_decay;
        std::string m_type;
};