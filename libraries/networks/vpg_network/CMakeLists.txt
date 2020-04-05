cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(vpg_network)

find_package(Torch REQUIRED)

# Paths to include vpg_parameters.hpp and global_parameters.hpp for global settings
set(CONTROLLER_PATH ../../../controllers/vpg_controller)
set(PARAMETERS_PATH ../../../controllers)

# Specify target as shared object 
add_library(vpg_network SHARED vpg_network.cpp vpg_episode.cpp) 

# Include the directory of the controller 
target_include_directories(vpg_network PRIVATE ${CONTROLLER_PATH} ${PARAMETERS_PATH}) 

# Link the PyTorch libraries
target_link_libraries(vpg_network "${TORCH_LIBRARIES}")

set_property(TARGET vpg_network PROPERTY CXX_STANDARD 11)

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET vpg_network
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:vpg_network>)
endif (MSVC)
