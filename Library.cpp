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

LZ4Compress::LZ4Compress(std::vector<unsigned char> data) : data(data) {}

int LZ4Compress::getCompressedDataSize(int uncompressedDataSize) {
    return LZ4_compressBound((int) uncompressedDataSize);
}

vector<unsigned char> LZ4Compress::Compress() {
    vector<unsigned char> uncompressedData = this->data;
    allocator<unsigned char> bufferAllocator;
    unique_ptr<unsigned char> dataBuffer =
            bufferAllocator.allocate((unsigned long) this->getCompressedDataSize((int)uncompressedData.size()));

    int compressedSize = LZ4_compress_default((char *) uncompressedData.data(),
                         (char *) dataBuffer.get(),
                         (int) uncompressedData.size(),
                         (int) uncompressedData.size());
    if(compressedSize == -1) {
        throw Exception();
        return NULL;
    }
    vector<unsigned char> compressedData((unsigned int) compressedSize);
    for(unsigned int i =0; i < compressedSize; i++) {
        compressedData[i] = dataBuffer.get()[i];
    }
    return compressedData;
}

vector<unsigned char> LZ4Compress::Decompress(unsigned int originalSize) {
    vector<unsigned char> compressedData = this->data;
    unique_ptr<unsigned char> buffer = allocator<unsigned char>().allocate(originalSize);
    LZ4_decompress_fast((char *)compressedData.data(), (char *) buffer.get(), originalSize);
    vector<unsigned char> uncompressedData(originalSize);
    for(int i = 0; i < originalSize; i++ ) {
        uncompressedData[i] = buffer.get()[i];
    }
    return uncompressedData;
}

Server::Server(unsigned short serverPort)
        : serverPort(serverPort), serverSocket(serverPort) {}

Mat Server::Receive() {
    string sourceAddress;
    unsigned short sourcePort;
    do {
        recvMsgSize = serverSocket.recvFrom(buffer, eBufferLength, sourceAddress, sourcePort);
    } while (recvMsgSize > sizeof(int));
    int packet_n = ((int *) buffer)[0];
    cout << "Expected Packet Length" << packet_n;
    char *longbuf = new char[ePacketSize * packet_n];
    for (int i = 0; i < packet_n; i++) {
        recvMsgSize = serverSocket.recvFrom(buffer, eBufferLength, sourceAddress, sourcePort);
        if (recvMsgSize != ePacketSize) {
            cerr << "Received unexpected size pack:" << recvMsgSize << endl;
            continue;
        }
        memcpy(&longbuf[i * ePacketSize], buffer, ePacketSize);
    }
    cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;

    Mat rawData = Mat(1, ePacketSize * packet_n, CV_8UC1, longbuf);
    Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
    if (frame.size().width == 0) {
        cerr << "Image decode failure!" << endl;
    }
    free(longbuf);
    return frame;
}

void Server::Show(Mat &m) {
    namedWindow("Server image window", CV_WINDOW_AUTOSIZE);
    imshow("server image window", m);
}

void Server::ShowReceiveBlocking() {

    cout << "Entered ShowReceiveBlocking()\n";

    unsigned short servPort = SHORT_CLIENT_PORT; // First arg:  local port

    namedWindow("recv", CV_WINDOW_AUTOSIZE);
    try {
        UDPSocket sock(servPort);

        char buffer[BUF_LEN]; // Buffer for echo string
        int recvMsgSize; // Size of received message
        string sourceAddress; // Address of datagram source
        unsigned short sourcePort; // Port of datagram source

        clock_t last_cycle = clock();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while (1) {
            // Block until receive message from a client
            do {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            } while (recvMsgSize > sizeof(int));
            int total_pack = ((int *) buffer)[0];
            cout << "expecting length of packs:" << total_pack << endl;
            char *longbuf = new char[PACK_SIZE * total_pack];
            for (int i = 0; i < total_pack; i++) {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                if (recvMsgSize != PACK_SIZE) {
                    cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                    continue;
                }
                memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
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
            cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8)
                 << endl;

            cout << next_cycle - last_cycle;
            last_cycle = next_cycle;
        }
#pragma clang diagnostic pop
    } catch (SocketException &e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

void Server::WriteStreamedFrame(std::string path) {
    Mat m = this->Receive();
    imwrite(path, m);
}

void Server::ShowAndWrite(std::string path) {
    Mat m = this->Receive();
    imwrite(path, m);
    this->Show(m);
}

Server::~Server() {

    this->serverSocket.disconnect();
    this->serverSocket.cleanUp();
    // this->serverSocket.~Socket(); commented out for osx compatibility

}

void Server::Close() {
    this->~Server();
}

Client::Client(std::string server, unsigned short port, int cameraNumber, int imageQuality, int imageW, int imageH)
        : destAddr(server),
          destPort(port),
          v(cameraNumber),
          imageQuality(imageQuality),
          imageW(imageW),
          imageH(imageH),
          clientSocket() {}

vector<unsigned char> Client::compress(cv::Mat m) {
    int jpeg_quality = this->imageQuality;

    vector<uchar> encoded_image;
    vector<int> compression_params;
    compression_params.push_back((int &&) CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(jpeg_quality);
    imencode(".jpg", m, encoded_image, compression_params);
    return encoded_image;
}


Mat Client::Send() {
    std::cout << "preparing one frame..." << std::endl;
    try {
        Mat frame, send;
        if (!v.isOpened()) throw std::runtime_error("Camera is not opened.");
        // now stream a frame
        v >> frame;
        if (frame.size().width == 0); // integrity check (skip errors)
        // normalize our image
        resize(frame, send, Size(this->imageW, this->imageH), 0, 0, INTER_LINEAR);
        // set compression
        vector<uchar> encoded = compress(send);
        // calculate packet size
        int total_pack = (int) (1 + (encoded.size() - 1) / ePacketSize);
        // send packet size
        int ibuf[1];
        ibuf[0] = total_pack;
        clientSocket.sendTo(ibuf, sizeof(int), this->destAddr, this->destPort);
        // send fragmentations
        for (int i = 0; i < total_pack; i++)
            clientSocket.sendTo(&encoded[i * ePacketSize], ePacketSize, this->destAddr, this->destPort);
        std::cout << "\tsent." << std::endl;
        return frame;
    } catch (Exception &e) {
        cerr << e.what() << endl;
    }
}

vector<unsigned char> Client::Send(Mat &frame) {
    std::cout << "sending one frame..." << std::endl;
    try {
        Mat send;
        if (frame.size().width == 0); // integrity check (skip errors)
        // normalize our image
        resize(frame, send, Size(this->imageW, this->imageH), 0, 0, INTER_LINEAR);
        // set compression
        vector<unsigned char> encoded = compress(send);
        // calculate packet size
        int total_pack = (int) (1 + (encoded.size() - 1) / ePacketSize);
        // send packet size
        int ibuf[1];
        ibuf[0] = total_pack;
        clientSocket.sendTo(ibuf, sizeof(int), this->destAddr, this->destPort);
        // send fragmentations
        for (int i = 0; i < total_pack; i++)
            clientSocket.sendTo(&encoded[i * ePacketSize], ePacketSize, this->destAddr, this->destPort);
        std::cout << "\tsent." << std::endl;
        return encoded;
    } catch (Exception &e) {
        cerr << e.what() << endl;
    }
}

void Client::Send(vector<unsigned char> &encoded) {
    std::cout << "sending one frame..." << std::endl;
    try {
        // calculate packet size
        int total_pack = (int) (1 + (encoded.size() - 1) / ePacketSize);
        // send packet size
        int ibuf[1];
        ibuf[0] = total_pack;
        clientSocket.sendTo(ibuf, sizeof(int), this->destAddr, this->destPort);
        // send fragmentations
        for (int i = 0; i < total_pack; i++)
            clientSocket.sendTo(&encoded[i * ePacketSize], ePacketSize, this->destAddr, this->destPort);
        std::cout << "\tsent." << std::endl;
    } catch (Exception &e) {
        cerr << e.what() << endl;
    }
}

Mat Client::Capture() {
    Mat frame;
    if (!v.isOpened()) throw std::runtime_error("Camera is not opened.");
    // now stream a frame
    v >> frame;
    if (frame.size().width == 0); // integrity check (skip errors)
    return frame;
}

void Client::Write(string path, cv::Mat& frame) {
    imwrite(path, frame);
}

void Client::Write(string path, vector<unsigned char> v) {
    ofstream out(path.data(), ios::out | ios::binary);
    const char *p = reinterpret_cast<const char *>(v.data());
    out.write(p, v.size() * sizeof(char));
}

void Client::WriteAndSend(string path, cv::Mat& frame) {
    imwrite(path, frame);
    this->Send(frame);
}

void Client::WriteAndSend(string path, vector<unsigned char> v) {
    ofstream out(path.data(), ios::out | ios::binary);
    const char *p = reinterpret_cast<const char *>(v.data());
    out.write(p, v.size() * sizeof(char));
    this->Send(v);
}

Client::~Client() {
    this->clientSocket.disconnect();
    this->clientSocket.cleanUp();
    //this->clientSocket.~Socket(); // commented out for osx compatibility
}

void Client::Close() {
    this->~Client();
}

/*
 * ===================== IMPLEMENTATIONS ===========================
 */

Client *__n_client_global;
Server *__n_server_global;

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_initialize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1initialize
        (JNIEnv *env, jobject obj, jint _j_mode) {
    int mode = (int) _j_mode;
    if (mode == 0) { // client
        __n_client_global = new Client(SERVER_ADDRESS,
                                       SHORT_SERVER_PORT,
                                       eDefaultCameraNumber, eEncodeQuality, eDefaultFrameWidth, eDefaultFrameHeight);
    } else if (mode == 1) {
        __n_server_global = new Server(SHORT_SERVER_PORT);
    } else {
        cerr << "UDPStreamer native initialization failed: no such mode as" << mode << endl;
    }
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_init_client
 * Signature: (Ljava/lang/String;SIIII)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1init_1client
        (JNIEnv *env,
         jobject obj,
         jstring __serverAddress,
         jshort __serverPort,
         jint __cameraNumber,
         jint __encodeQuality,
         jint __frameWidth,
         jint __frameHeight) {
    const char *serverAddress_cp = (const char *) __serverAddress;
    string serverAddress = string(serverAddress_cp);
    unsigned short serverPort = (unsigned short) __serverPort;
    int cameraNumber = (int) __cameraNumber;
    int encodeQuality = (int) __encodeQuality;
    int frameWidth = (int) __frameWidth;
    int frameHeight = (int) __frameHeight;
    __n_client_global = new Client(serverAddress, serverPort, cameraNumber, encodeQuality, frameWidth, frameHeight);
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_init_server
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1init_1server
        (JNIEnv *env, jobject obj, jshort __j_serverPort) {
    unsigned short sp = (unsigned short) __j_serverPort;
    __n_server_global = new Server(sp);
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1close
        (JNIEnv *env, jobject obj, jint _j_mode) {
    __n_client_global->Close();
    __n_server_global->Close();
    delete __n_client_global;
    delete __n_server_global;
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_grabFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1grabFrame
        (JNIEnv *env, jobject obj) {
    Mat m = __n_client_global->Send();
    std::cout << "Mat m is allocated." << std::endl;
    int rows = m.rows;
    int cols = m.cols;
    int num_el = rows * cols;
    size_t len = num_el * m.elemSize1();
    std::cout << len << std::endl;
    jbyteArray __ba = env->NewByteArray((jsize) len);
    std::cout << "Byte array allocated" << std::endl;
    env->SetByteArrayRegion(__ba, 0, (jsize) len, reinterpret_cast<jbyte *>(m.data));
    std::cout << "Byte array inserted. Length " << (unsigned int) env->GetArrayLength(__ba) << std::endl;
    return __ba;
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_streamFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1streamFrame
        (JNIEnv *env, jobject obj) {
    Mat m = __n_client_global->Capture();
    std::cout << "Mat m is allocated." << std::endl;
    int rows = m.rows;
    int cols = m.cols;
    int num_el = rows * cols;
    size_t len = num_el * m.elemSize1();
    std::cout << len << std::endl;
    jbyteArray __ba = env->NewByteArray((jsize) len);
    std::cout << "Byte array allocated" << std::endl;
    env->SetByteArrayRegion(__ba, 0, (jsize) len, reinterpret_cast<jbyte *>(m.data));
    std::cout << "Byte array inserted. Length " << (unsigned int) env->GetArrayLength(__ba) << std::endl;
    return __ba;
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_sendFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1sendFrame
        (JNIEnv *env, jobject obj) {
    __n_client_global->Send();
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_sendCustomFrame
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1sendCustomFrame
        (JNIEnv *env, jobject obj, jbyteArray __j_barr) {
    unsigned int len = (unsigned int) env->GetArrayLength(__j_barr);
    vector<unsigned char> img_uc(len);
    jbyte *b = (jbyte *) env->GetByteArrayElements(__j_barr, NULL);
    for (int i = 0; i < len; i++) {
        img_uc[i] = (unsigned char) b[i];
    }
    __n_client_global->Send(img_uc);

}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Server_showFrame_blocking
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Server_1showFrame_1blocking
        (JNIEnv *env, jobject obj) {
    __n_server_global->ShowReceiveBlocking();
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Server_retrieveFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_in_derros_jni_UDPStreamer__1n_1Server_1retrieveFrame
        (JNIEnv *env, jobject obj) {
    Mat m = __n_server_global->Receive();
    std::cout << "Mat m is allocated." << std::endl;
    int rows = m.rows;
    int cols = m.cols;
    int num_el = rows * cols;
    size_t len = num_el * m.elemSize1();
    std::cout << len << std::endl;
    jbyteArray __ba = env->NewByteArray((jsize) len);
    std::cout << "Byte array allocated" << std::endl;
    env->SetByteArrayRegion(__ba, 0, (jsize) len, reinterpret_cast<jbyte *>(m.data));
    std::cout << "Byte array inserted. Length " << (unsigned int) env->GetArrayLength(__ba) << std::endl;
    return __ba;
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_writeFrame
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1writeFrame
        (JNIEnv *env, jobject obj, jstring __j_str) {
    string path(reinterpret_cast<const char *>(__j_str));
    __n_client_global->Write(path, __n_client_global->Capture());
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_writeAndSend
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1writeAndSend
        (JNIEnv *env, jobject obj, jstring __j_str) {
    string path(reinterpret_cast<const char *>(__j_str));
    __n_client_global->WriteAndSend(path, __n_client_global->Capture());
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_writeCustomFrame
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1writeCustomFrame
        (JNIEnv *env, jobject obj, jstring __j_str, jbyteArray __j_ba) {
    string path(reinterpret_cast<const char *>(__j_str));
    unsigned int len = (unsigned int) env->GetArrayLength((jarray) __j_ba);
    jbyte *b = (jbyte *) env->GetByteArrayElements(__j_ba, NULL);
    vector<unsigned char> img(len);
    for(unsigned int i = 0; i < len; i ++) {
        img[i] = (unsigned char)(b[i]); // reinterpret_cast won't work, converts a signed to unsigned
    }
    __n_client_global->Write(path, img);
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Client_writeAndSendCustom
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Client_1writeAndSendCustom
        (JNIEnv *env, jobject obj, jstring __j_str, jbyteArray __j_ba) {
    string path(reinterpret_cast<const char *>(__j_str));
    unsigned int len = (unsigned int) env->GetArrayLength((jarray) __j_ba);
    jbyte *b = (jbyte *) env->GetByteArrayElements(__j_ba, NULL);
    vector<unsigned char> img(len);
    for(unsigned int i = 0; i < len; i ++) {
        img[i] = (unsigned char)(b[i]); // reinterpret_cast won't work, converts a signed to unsigned
    }
    __n_client_global->WriteAndSend(path, img);
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Server_writeRetrivedFrame
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Server_1writeRetrivedFrame
        (JNIEnv *env, jobject obj, jstring __j_str) {
    string path(reinterpret_cast<const char *>(__j_str));
    __n_server_global->WriteStreamedFrame(path);
}

/*
 * Class:     in_derros_jni_UDPStreamer
 * Method:    _n_Server_showAndWriteFrame
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_in_derros_jni_UDPStreamer__1n_1Server_1showAndWriteFrame
        (JNIEnv *env, jobject obj, jstring __j_str) {
    string path(reinterpret_cast<const char *>(__j_str));
    __n_server_global->ShowAndWrite(path);
}