#include <webots/motor.h>
#include <webots/robot.h>

#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>

#include <iostream>
#include <vector> 

namespace k4{

    #define MAX_SPEED 47.6
    #define REDUCER   0.5 // Only use 50% of max speed 

    #define IR_MIN 100.0  // No obstacle
    #define IR_MAX 1000.0 // Almost touching obstacle 

    #define NUMBER_OF_INFRARED_SENSORS 12
    #define NUMBER_OF_PERIMETER_IR_SENSORS 8

    /**
     * Initialize the robot's infrared sensors, motors and LEDs 
     */
    int init_robot();

    /**
     * Perform a time step 
     */
    int khepera_running();

    /**
     * Set the left and right motor speeds. 
     */
    void set_motors_speed(double left, double right);

    /**
     * Get the left and right motor speeds. 
     */
    std::vector<double> get_motors_speed();

    /**
     * Set the left and right motor speeds. Input must be 
     * normalized between [-1,1]. Backward: -1, Forward: +1
     */
    void set_normalized_motors_speed(double left, double right);

    /**
     * Get the left and right normalized motor speeds. 
     */
    std::vector<double> get_normalized_motors_speed();

    /**
     * Normalize a range reading between 0 and 1. 
     * 0 (no obstacle), 1 (obstacle very close)
     */
    double normalize_range(double range);

    /**
     * Get the range of infrared sensor i
     */
    double get_range(int i);

    /**
     * Get the range of each infrared sensor 
     */
    std::vector<double> get_ranges();

    /**
     * Get the normalized range of each infrared sensor. 
     * Returns values between 0 (no obstacle) and 1 (obstacle very close)
     */
    std::vector<double> get_normalized_ranges();

}