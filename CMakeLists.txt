
cmake_minimum_required(VERSION 2.8)

project( jni_udp_streaming_lib )

set(CMAKE_BUILD_TYPE Release)

find_package( OpenCV REQUIRED )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library( jni_udp_streaming_lib SHARED Library.cpp PracticalSocket.cpp)

target_link_libraries( jni_udp_streaming_lib ${OpenCV_LIBS} )