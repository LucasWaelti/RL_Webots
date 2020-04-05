#pragma once

#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include <time.h>

#include <webots/supervisor.h>

/**
 * Detect if the robot hit something. 
 * @output (bool) Whether the robot is currently touching something. 
 */
bool detect_collision();

/**
 * Detect if the simulation exploded. Problem occuring in 
 * Webots version R2019b and previous. Solved in R2020a. 
 */
bool simulation_exploded(); 

/**
 * Adds the speeds set to compute the "distance" travelled. 
 */
void measure_travelled_distance(double left, double right);

/**
 * Reset the measured travelled distance. 
 */
void reset_distance();

/**
 * Get the measured travelled distance. 
 */
double get_distance();

/**
 * Move the robot at a pre-defined random location. 
 */
void supervisor_move_robot();