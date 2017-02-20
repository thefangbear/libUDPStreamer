/*
 * Created by Alex Fang on 2/20/17.
 * Copyright (c) 2017 Conceptual Inertia, Inc. All rights reserved.
 *
 * License included in project root folder.
 * Legal Contact: Conceptual-Inertia@magikpns.com
 */

#ifndef LAN_VID_PSEUDOSTREAM_LIBRARY_H
#define LAN_VID_PSEUDOSTREAM_LIBRARY_H

#include "opencv2/opencv.hpp"
#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#define SERVER_PORT "1111"
#define SERVER_ADDRESS "localhost"
#define CAMERA_NUMBER 0
#define BUF_LEN 65540 // Larger than maximum UDP packet size
vector < uchar > compress(cv::Mat m);
void native_blocking_receive();
void native_send();

#endif //LAN_VID_PSEUDOSTREAM_LIBRARY_H
