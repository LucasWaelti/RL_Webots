#!/bin/bash 
# Setup for running khepera IV robots and RL

echo Exporting environment variables
export WEBOTS_HOME=/usr/local/webots ; echo WEBOTS_HOME=$WEBOTS_HOME
export WEBOTS_HOME_PATH=$WEBOTS_HOME 

# Change this path to point to your libtorch installation 
export LIBTORCH_PATH=/home/$(whoami)/Documents/PyTorch/libtorch
echo LIBTORCH_PATH=$LIBTORCH_PATH
