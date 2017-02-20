

#include "PracticalSocket.h"      // For UDPSocket and SocketException
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()

using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

#define SERVER_ADDRESS "localhost"
#define SERVER_PORT "1111"
#define CAMERA_NUMBER 0

vector < uchar > compress(Mat m) {
    int jpeg_quality = ENCODE_QUALITY;

    vector < uchar > encoded_image;
    vector < int > compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(jpeg_quality);
    imencode(".jpg", m, encoded_image, compression_params);
    return encoded_image;
}

void native_send() {
    string serverAddr = SERVER_ADDRESS;
    unsigned short serverPort = Socket::resolveService(SERVER_PORT, "udp");
    try {
        UDPSocket sock;
        Mat frame, send;
        VideoCapture camera( CAMERA_NUMBER );
        if(!camera.isOpened()) throw std::runtime_error("Camera is not opened.");
        // now stream a frame
        camera >> frame;
        if(frame.size().width == 0) {} // integrity check (skip errors)
        // normalize our image
        resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
        // set compression
        vector < uchar > encoded = compress(send);
        // calculate packet size
        int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;
        // send packet size
        int ibuf[1];
        ibuf[0] = total_pack;
        sock.sendTo(ibuf, sizeof(int), serverAddr, serverPort);
        // send fragmentations
        for (int i = 0; i < total_pack; i++)
            sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, serverAddr, serverPort);

    } catch (SocketException & e) {
        cerr << e.what() << endl;
    }
}
