// File:        khepera_test.cpp
// Date:        28.03.2020  
// Description: Simple demo controller showing basic controls 
// Author:      Lucas Waelti
// Modifications:

/*
 * This controller was readapted and modified from the default 
 * khepera.c controller provided by Cyberbotics. 
 */

#include <khepera.hpp> 

int main(int argc, char **argv) {
  
    // Initililaize the robot 
    int time_step = k4::init_robot(); 

    // store the last time a message was displayed
    int last_display_second = 0;

    std::vector<double> ranges, normalized_ranges; 

    // main loop
    while (wb_robot_step(time_step) != -1) {

        // Get infrared data 
        normalized_ranges = k4::get_normalized_ranges(); 
        ranges = k4::get_ranges(); 

        // Display the normalized front sensor reading
        if(last_display_second != (int)wb_robot_get_time()){
            last_display_second = (int)wb_robot_get_time();
            std::cout << "Distance front: " << normalized_ranges[3] << std::endl;
        }
        
        // simple obstacle avoidance algorithm
        // based on the front infrared sensors
        double speed_offset = 0.2 * (MAX_SPEED - 0.03 * ranges[3]);
        double speed_delta = 0.03 * ranges[2] - 0.03 * ranges[4];
        k4::set_motors_speed(speed_offset + speed_delta, speed_offset - speed_delta);
    };

    wb_robot_cleanup();

    return EXIT_SUCCESS;
}
