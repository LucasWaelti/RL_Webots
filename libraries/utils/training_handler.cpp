#include "training_handler.hpp"

static const char *def = "K4"; // Robot def 

bool detect_collision(){
    // Supervisor - get handle on robot position 
    WbNodeRef robot_node_ref = wb_supervisor_node_get_from_def(def);
    int num_contact_points = wb_supervisor_node_get_number_of_contact_points(robot_node_ref);
    if(num_contact_points > 0)
        return true; 
    else
        return false; 
}

bool simulation_exploded(){
    // Supervisor - get handle on robot position 
    WbNodeRef robot_node_ref = wb_supervisor_node_get_from_def(def);
    WbFieldRef position_field = wb_supervisor_node_get_field(robot_node_ref, "translation");
    double y_pose =  wb_supervisor_field_get_sf_vec3f(position_field)[1];
    return (y_pose > 0.01);
}

static double distance = 0.0; 

void measure_travelled_distance(double left, double right){
    distance += (left + right)/1000.0; 
}

void reset_distance(){
    distance = 0.0; 
}

double get_distance(){
    return distance; 
}

static std::default_random_engine _generator;
void supervisor_move_robot(){

    // Supervisor - get handle on robot position 
    WbNodeRef robot_node_ref = wb_supervisor_node_get_from_def(def);
    WbFieldRef position_field = wb_supervisor_node_get_field(robot_node_ref, "translation");
    //WbFieldRef rotation_field = wb_supervisor_node_get_field(robot_node_ref, "rotation");

    // Define some locations 
    std::vector<double*> locations;
    double loc0[3] = {-0.66,0,-0.68};   // Upper left
    double loc1[3] = {-0.79,0,0.79};    // Lower left
    double loc2[3] = {0,0,0};           // Middle
    double loc3[3] = {0.71,0,0.72};     // In White area
    locations.push_back(loc0);
    locations.push_back(loc1);
    locations.push_back(loc2);
    locations.push_back(loc3);

    // Sample a location
    std::uniform_int_distribution<> dis(0, locations.size()-1);
    int ind = dis(_generator);

    // Spawn the robot at chosen location
    wb_supervisor_field_set_sf_vec3f(position_field, locations[ind]);
}