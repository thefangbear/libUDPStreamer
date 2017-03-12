# JNI UDP Live Image Streaming Library
This library is a fork of the UDP Live Image Streaming library by chenxiaoqino. The main() functions in Server.cpp and Client.cpp were combined into one library called Library.cpp, and a JNI wrapper is wrapped around it for use in Java code. This fork is mainly prepared for Abington Friends School's Roobotics Robotics Team's entry into the 2017 FIRST Robotics Competition.

## Documentation
![how](https://github.com/thefangbear/libUDPStreamer/raw/master/udpstreamer.png)
Please read our excellent [Java API Documentation and tutorial](https://github.com/thefangbear/udp-img-lib/wiki/Java-API-Documentation).

## Release

A Maven release is available (Thanks to Roobotics) at https://github.com/Roobotics-FRC/RooMaven. The current version is 0.2.1.

## Author
The library is originally written by [chenxiaoqino](//github.com/chenxiaoqino) but later revised to add JNI, multiple new classes, and restructured by Alex Fang.

## License
Licensed under the MIT License.

<hr>

# ORIGINAL README
The readme below is exerpted from chenxiaoqino's README for his C++ application:

## UDP Live Image Streaming

This project is inspired by https://www.cs.utexas.edu/~teammco/misc/udp_video/ , where images are grabbed from camera on one machine and transfered to another machine via UDP, resulting in negligible latency.

Parameters such as stream size or quality can be adjusted in `config.h` before re-compile the program.

### Grabbing

The code grabs video stream from OpenCV's default input video device. To change input source, simply change the argument passed to `cv::VideoCapture()` accordingly.

### Encoding

To avoid latency altogether there is no video codec involved during this process. Every frame is individually encoded to `jpeg` format by OpenCV to drastically reduce the bandwidth consumption.

If passing raw image is preferred, consider changing `jpeg` to `bmp`.

<hr>
