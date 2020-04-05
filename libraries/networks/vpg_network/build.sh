# Build the target 
mkdir build 
cd build
cmake -DCMAKE_PREFIX_PATH=$LIBTORCH_PATH ..
make

# Move the generated shared object to the libraries folder of the Webots project
cp ./libvpg_network.so ../../..

# Move the required headers as well
cp ../vpg_wrapper.hpp ../../..
echo '// Do not modify! This file is generated when compiling the `libvpg_network.so` file.' | cat - ../../../vpg_wrapper.hpp > temp && mv temp ../../../vpg_wrapper.hpp
cp ../vpg_episode.hpp ../../..
echo '// Do not modify! This file is generated when compiling the `libvpg_network.so` file.' | cat - ../../../vpg_episode.hpp > temp && mv temp ../../../vpg_episode.hpp
