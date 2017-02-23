
cmake_minimum_required(VERSION 2.8)

project( UDPStreamer )

set(CMAKE_BUILD_TYPE Release)

find_package(JNI REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library( UDPStreamer SHARED Library.cpp PracticalSocket.cpp)

target_link_libraries( UDPStreamer ${OpenCV_LIBS} )