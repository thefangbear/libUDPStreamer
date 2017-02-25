/*
 * Created by Alex Fang on 2/20/17.
 * Copyright (c) 2017 Conceptual Inertia, Inc. All rights reserved.
 *
 * License included in project root folder.
 * Legal Contact: Conceptual-Inertia@magikpns.com
 */

#ifndef UDPSTREAMER_H
#define UDPSTREAMER_H

//========= INCLUDE ==============
#include "opencv2/opencv.hpp"
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
#define SHORT_SERVER_PORT 1111
#define SERVER_ADDRESS "localhost"
#define CAMERA_NUMBER 0
#define BUF_LEN 65540 // Larger than maximum UDP packet size

enum {
    // default settings
    eDefaultCameraNumber = 0,
    eDefaultFrameWidth = 1280,
    eDefaultFrameHeight = 720,
    ePacketSize = 4096,
    eEncodeQuality = 80,
    eBufferLength = 65540
};

void initialize(int);
class Server {
public:
    Server(unsigned short);
    cv::Mat Receive();
    void    Show(cv::Mat& m);
    void    ShowReceiveBlocking();
private:
    char buffer[ BUF_LEN ]; // Buffer for echo string
    int recvMsgSize; // Size of received message
    unsigned short serverPort; // Port of datagram source
    UDPSocket serverSocket;
};

class Client {
public:
    Client(std::string, unsigned short, int, int, int, int);
    cv::Mat Send();
    vector<unsigned char> Send(cv::Mat& m);
    void Send(vector<unsigned char>& m);
    cv::Mat Capture();

private:
    std::string destAddr;
    unsigned short destPort;
    UDPSocket clientSocket;
    cv::VideoCapture v;
    int imageQuality;
    int imageW;
    int imageH;
    std::vector<unsigned char> compress(cv::Mat m);
};

#endif // UDPSTREAMER_H
