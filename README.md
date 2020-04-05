# RL_Webots
This repository shows how Deep Reinforcement Learning can be used within Webots. This project was developed under Linux. While it should be possible to use it with other operating systems, this was not tested. 

The code presented in this repository was adapted from a semester project at the [Distributed Intelligent Systems and Algorithms Laboratory (DISAL)](https://www.epfl.ch/labs/disal/). A summary of the project can be found [here](https://disalw3.epfl.ch/teaching/student_projects/ay_2019-20/ws/DISAL-SP133_summary.pdf). The part presented here is about **Deep Reinforcement Learning**, leveraging **Policy Gradient** to train a fully connected, feed forward neural network to solve a simple **obstacle avoidance task** on a **continuous action space** with a Khepera IV robot. 

The project was entirely implemented in C++ and relies on [libtorch](https://pytorch.org/cppdocs/), [PyTorch](https://pytorch.org/)'s C++ API. 

## Install
Download [PyTorch's C++ API](https://pytorch.org/). Choose following options: 
- Build: Stable
- OS: Linux
- Package: LibTorch
- Language: C++
- CUDA: None

and click on the second link (cxx11 ABI, 87.4M). Extract the zip file wherever you like on your computer. You need to edit the `env.sh` script and adapt the path to your libtorch installation folder: 
```bash
export LIBTORCH_PATH=/path/to/libtorch 
```

Open up a terminal, go to the project folder and run the following commands to build the project: 
```bash
source env.sh
./build_project.sh
```
## Documentation
Please visit the repository's [wiki](https://github.com/LucasWaelti/RL_Webots/wiki) for more details. 