#!/bin/bash

#
# builds JNI Wrapper
#

mkdir build
cd build
cmake ../
make
cd ../
gcc -L/Users/derros/Projects/udp-image-streaming/build/libjni_udp_streaming_lib.dylib \
 -I/Library/Java/JavaVirtualMachines/jdk1.8.0_102.jdk/Contents/Home/include \
 -I/Library/Java/JavaVirtualMachines/jdk1.8.0_102.jdk/Contents/Home/include/darwin \
 UDPStreamerJNI.cpp 
