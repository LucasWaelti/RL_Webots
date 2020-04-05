#include "khepera.hpp"

static int time_step = 8; 

static const char *infrared_sensors_names[NUMBER_OF_INFRARED_SENSORS] = {
  // turret sensors
  "rear left infrared sensor", "left infrared sensor", "front left infrared sensor", "front infrared sensor",
  "front right infrared sensor", "right infrared sensor", "rear right infrared sensor", "rear infrared sensor",
  // ground sensors
  "ground left infrared sensor", "ground front left infrared sensor", "ground front right infrared sensor",
  "ground right infrared sensor"};

// Declare required device tags 
static WbDeviceTag leds[3]; 
static WbDeviceTag infrared_sensors[12];
static WbDeviceTag left_motor, right_motor;


int k4::init_robot(){
    wb_robot_init();

    time_step = (int)wb_robot_get_basic_time_step();

    // get and enable the infrared sensors
    for (int i = 0; i < 12; ++i) {
        infrared_sensors[i] = wb_robot_get_device(infrared_sensors_names[i]);
        wb_distance_sensor_enable(infrared_sensors[i], time_step);
    }

    // get the led actuators
    leds[0] = wb_robot_get_device("front left led"); 
    leds[1] = wb_robot_get_device("front right led");
    leds[2] = wb_robot_get_device("rear led");
    for (int i = 0; i < 3; ++i)
        wb_led_set(leds[i], 0xFFFFFF & rand());

    // get the motors and set target position to infinity (speed control)
    left_motor = wb_robot_get_device("left wheel motor");
    right_motor = wb_robot_get_device("right wheel motor");
    wb_motor_set_position(left_motor, INFINITY);
    wb_motor_set_position(right_motor, INFINITY);
    wb_motor_set_velocity(left_motor, 0.0);
    wb_motor_set_velocity(right_motor, 0.0);

    return time_step; 
}

int k4::khepera_running(){
    return (wb_robot_step(time_step) != -1) ? 1 : 0; 
}

void k4::set_motors_speed(double left, double right){
    wb_motor_set_velocity(left_motor, left);
    wb_motor_set_velocity(right_motor, right);
}

std::vector<double> k4::get_motors_speed(){
    std::vector<double> speed; 
    speed.push_back(wb_motor_get_velocity(left_motor));
    speed.push_back(wb_motor_get_velocity(right_motor));
    return speed; 
}

void k4::set_normalized_motors_speed(double left, double right){
    if(abs(left)>1 || abs(right)>1){
        std::cout << "ERROR in set_normalized_motors_speed: input must be in [-1,1]" <<std::endl; 
        set_motors_speed(0.0,0.0);
        return; 
    }
    set_motors_speed(left*MAX_SPEED*REDUCER,right*MAX_SPEED*REDUCER);
}

std::vector<double> k4::get_normalized_motors_speed(){
    std::vector<double> speed = get_motors_speed(); 
    speed[0] /= (MAX_SPEED*REDUCER);
    speed[1] /= (MAX_SPEED*REDUCER);
    return speed; 
}

double k4::normalize_range(double range){
    double offset = 50.0; 
    double value = (range - IR_MIN - offset)/(IR_MAX - IR_MIN - offset);
    if(value < 0.0)
        value = 0.0; 
    else if(value > 1.0)
        value = 1.0; 
    return value; 
}

double k4::get_range(int i){
    return wb_distance_sensor_get_value(infrared_sensors[i]);
}

std::vector<double> k4::get_ranges(){
    std::vector<double> ranges; 
    for(int i=0; i<NUMBER_OF_PERIMETER_IR_SENSORS; i++)
        ranges.push_back(wb_distance_sensor_get_value(infrared_sensors[i]));
    return ranges; 
}

std::vector<double> k4::get_normalized_ranges(){
    std::vector<double> ranges; 
    for(int i=0; i<NUMBER_OF_PERIMETER_IR_SENSORS; i++)
        ranges.push_back(normalize_range(wb_distance_sensor_get_value(infrared_sensors[i])));
    return ranges;
}