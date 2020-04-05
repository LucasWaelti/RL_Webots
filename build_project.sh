#!/bin/bash
echo ""
echo -e "\033[1;33mBuilding the PyTorch shared object...\033[0m"
cd - 
cd ./libraries/networks/vpg_network
./build.sh

echo ""
echo -e "\033[1;33mBuilding the webots controller...\033[0m"
cd - 
cd ./controllers/vpg_controller
make

cd - 

