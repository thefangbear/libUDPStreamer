
cmake_minimum_required(VERSION 3.5)

project( UDPStreamer )

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "--std=c++14")
set(CMAKE_CXX_STANDARD 14)
find_package(JNI REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library( UDPStreamer SHARED Library.cpp PracticalSocket.cpp lz4.c)

target_link_libraries( UDPStreamer ${OpenCV_LIBS} )