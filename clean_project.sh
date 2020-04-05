#!/bin/bash
echo ""
echo -e "\033[1;33mCleaning the PyTorch shared object...\033[0m"
cd - 
cd ./libraries/networks/vpg_network
rm -r build
cd ../..
rm libvpg_network.so
rm vpg_episode.hpp
rm vpg_wrapper.hpp

echo ""
echo -e "\033[1;33mCleaning the webots controller...\033[0m"
cd ../controllers/vpg_controller
make clean 

echo "done" 
