/*
 * Created by Alex Fang on 2/20/17.
 * Copyright (c) 2017 Conceptual Inertia, Inc. All rights reserved.
 *
 * License included in project root folder.
 * Legal Contact: Conceptual-Inertia@magikpns.com
 */

#ifndef LAN_VID_PSEUDOSTREAM_LIBRARY_H
#define LAN_VID_PSEUDOSTREAM_LIBRARY_H
//========= INCLUDE ==============
#include "opencv2/opencv.hpp"
#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include "java/in_derros_jni_UDPStreamer.h"
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
//========= CONFIG ===============
#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80
//========= NETWRK ===============
#define SERVER_PORT "1111"
#define SERVER_ADDRESS "localhost"
#define CAMERA_NUMBER 0
#define BUF_LEN 65540 // Larger than maximum UDP packet size
vector < uchar > compress(cv::Mat m);
void native_blocking_receive();
void native_send();
const unsigned char* native_byteMatSend();
#endif //LAN_VID_PSEUDOSTREAM_LIBRARY_H
