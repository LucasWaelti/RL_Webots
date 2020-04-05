// File:          vpg_controller.cpp
// Date:          28.10.2019
// Description:   Main file of the VPG implementation
// Author:        Lucas Wälti
// Modifications:

#include <vpg_arbitrator.hpp>

// The arguments of the main function can be specified by the
// "controllerArgs" field of the Robot node
int main(int argc, char **argv) {

  // Listen to arguments for mode 
  bool eval = true; 
  if(argc == 1){
    std::cout << "No argument, using default 'eval' mode. Usage is 'training' or 'eval'." << std::endl; 
  }else if(argc == 2){
    std::string mode = argv[1]; 
    if(mode == "training")
      eval = false; 
    else
      eval = true; 
  }

  // Create an arbitrator instance
  VPG_arbitrator arbitrator = VPG_arbitrator(); 
  arbitrator.mode(/*evaluating*/ eval, /*verbose*/ true);
  arbitrator.reset();

  std::cout << "Arbitrator launched in " << (eval ? "evaluation" : "training") << " mode" << std::endl;
  
  while (arbitrator.step()) {
    // Check if simulation break
    if(simulation_exploded()){
      std::cout << "Simulation exploded." << std::endl;
      wb_supervisor_simulation_reset();
    }
  };

  if(arbitrator.is_training())
    wb_supervisor_simulation_quit(EXIT_SUCCESS); // ask Webots to terminate

  // cleanup code 
  return 0;
}
