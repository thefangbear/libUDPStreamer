/*
 * Created by Alex Fang on 2/20/17.
 * Copyright (c) 2017 Conceptual Inertia, Inc. All rights reserved.
 *
 * License included in project root folder.
 * Legal Contact: Conceptual-Inertia@magikpns.com
 */

#include "Library.h"

using namespace cv;
using namespace std;

vector<uchar> compress(Mat m) {
    int jpeg_quality = ENCODE_QUALITY;

    vector < uchar > encoded_image;
    vector < int > compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(jpeg_quality);
    imencode(".jpg", m, encoded_image, compression_params);
    return encoded_image;
}

void native_blocking_receive() {
    unsigned short servPort = atoi( SERVER_PORT ); // First arg:  local port

    namedWindow("recv", CV_WINDOW_AUTOSIZE);
    try {
        UDPSocket sock(servPort);

        char buffer[BUF_LEN]; // Buffer for echo string
        int recvMsgSize; // Size of received message
        string sourceAddress; // Address of datagram source
        unsigned short sourcePort; // Port of datagram source

        clock_t last_cycle = clock();

        while (1) {
            // Block until receive message from a client
            do {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            } while (recvMsgSize > sizeof(int));
            int total_pack = ((int * ) buffer)[0];

            cout << "expecting length of packs:" << total_pack << endl;
            char * longbuf = new char[PACK_SIZE * total_pack];
            for (int i = 0; i < total_pack; i++) {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                if (recvMsgSize != PACK_SIZE) {
                    cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                    continue;
                }
                memcpy( & longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
            }

            cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;

            Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
            Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
            if (frame.size().width == 0) {
                cerr << "decode failure!" << endl;
                continue;
            }
            imshow("recv", frame);
            free(longbuf);

            waitKey(1);
            clock_t next_cycle = clock();
            double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_SEC;
            cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

            cout << next_cycle - last_cycle;
            last_cycle = next_cycle;
        }
    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }
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

const unsigned char* toByteArray(const Mat& m) {
    return m.data;
}

const Mat native_Mat_send() {
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
        return frame;
    } catch (SocketException & e) {
        cerr << e.what() << endl;
    }
}

const unsigned char* native_byteMatSend() {
    return toByteArray(native_Mat_send());
}

