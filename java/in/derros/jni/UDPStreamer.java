package in.derros.jni;

// UDPStreamer JNI Interface
// @author R-J Alexander Fang
//

public class UDPStreamer {

    //===========================INIT FUNCTION=========================

    private native void _n_initialize(int _mode);

    private native void _n_init_client(String _address, short _port,
                                       int _camNumber, int _encodeQuality,
                                       int _frameWidth, int _frameHeight);

    private native void _n_init_server(short _port);

    private native void _n_close(int _mode);

    //
    //===========================CLIENT================================
    //

    /**
     * _n_grabFrame: Returns a camera frame for manipulation after send
     *
     * @return: byte[]
     */
    private native byte[] _n_Client_grabFrame();

    /**
     * _n_Client_streamFrame: only returns a frame without sending
     */
    private native byte[] _n_Client_streamFrame();

    /**
     * _n_Client_sendFrame: only sends a frame (w.o. returning data)
     */
    private native void _n_Client_sendFrame();

    /**
     * _n_sendCustomFrame: sends a custom frame
     */
    private native void _n_Client_sendCustomFrame(byte[] bs);

    /**
     * _n_Client_writeFrame: writes a camera frame to disk
     * @param path
     */
    private native void _n_Client_writeFrame(String path);

    /**
     * _n_Client_writeAndSend: writes and sends a camera frame over network and to disk
     * @param path
     */
    private native void _n_Client_writeAndSend(String path);

    /**
     * _n_Client_write: writes a frame to disk
     */
    private native void _n_Client_writeCustomFrame(String path, byte[] bs);


    /**
     * _n_Client_writeAndSend: writes and sends a frame over network
     */
    private native void _n_Client_writeAndSendCustom(String path, byte[] bs);

    //
    //===========================SERVER=================================
    //

    /**
     * Show frame using highgui natively implemented
     */
    private native void _n_Server_showFrame_blocking();


    /**
     * _n_retrieveFrame: retrieves a frame in byte array
     */
    private native byte[] _n_Server_retrieveFrame();

    private native void _n_Server_writeRetrivedFrame(String path);

    private native void _n_Server_showAndWriteFrame(String path);


	/*==========================  Java APIs ============================
     *==================================================================
	 */

    private int mode; // 0 = client, 1 = server

    public UDPStreamer(String lp, String mode) {
        System.load(lp);
        if (mode.equals("client")) {
            this.mode = 0;
        } else {
            this.mode = 1;
        }
        _n_initialize(this.mode);
    }

    public UDPStreamer(String lp,
                       String serverAddress,
                       short serverPort,
                       int cameraNumber,
                       int encodeQuality,
                       int frameWidth,
                       int frameHeight) {
        System.load(lp);
        _n_init_client(serverAddress, serverPort, cameraNumber, encodeQuality, frameWidth, frameHeight);
    }

    public UDPStreamer(String lp, short portOfThisServer) {
        System.load(lp);
        _n_init_server(portOfThisServer);
    }

    public UDPStreamer(String libraryName, String mode, boolean useSystemLibraryPath) {
        System.loadLibrary(libraryName);
        if (mode.equals("client")) {
            this.mode = 0;
        } else {
            this.mode = 1;
        }
        _n_initialize(this.mode);
    }

    public UDPStreamer(String libraryName,
                       String serverAddress,
                       short serverPort,
                       int cameraNumber,
                       int encodeQuality,
                       int frameWidth,
                       int frameHeight,
                       boolean useSystemLibraryPath=true) {
        System.loadLibrary(libraryName);
        _n_init_client(serverAddress, serverPort, cameraNumber, encodeQuality, frameWidth, frameHeight);
    }

    public UDPStreamer(String libraryName, short portOfThisServer, boolean useSystemLibraryPath) {
        System.loadLibrary(libraryName);
        _n_init_server(portOfThisServer);
    }

    public void close() {
        _n_close(this.mode);
    }

    public void finalize() {
        this.close();
    }

    // Generic
    public byte[] genericGrabFrame() {
        if (this.mode == 0) {
            return _n_Client_grabFrame();
        } else {
            return _n_Server_retrieveFrame();
        }
    }

    public void genericWriteAndSendFrame(String path) {
        if(this.mode == 0) {
            _n_Client_writeAndSend(path);
        } else {
            _n_Server_writeRetrivedFrame(path);
        }
    }

    //=========CLIENT===========
    //

    public byte[] clientGrabFrame() {
        assert this.mode == 0;
        return _n_Client_grabFrame();
    }

    public void clientSendOneFrame() {
        assert this.mode == 0;
        _n_Client_sendFrame();
    }

    public byte[] clientStreamFrame() {
        assert this.mode == 0;
        return _n_Client_streamFrame();
    }

    public void clientBlockingSendFrame() {
        assert this.mode == 0;
        while (true) {
            _n_Client_sendFrame();
        }
    }

    public void clientSendCustomFrame(byte[] bs) {
        assert this.mode == 0;
        _n_Client_sendCustomFrame(bs);
    }

    public void clientWriteAndSendFrame(String path) {
        assert this.mode == 0;
        _n_Client_writeAndSend(path);
    }

    public void clientWriteFrame(String path) {
        assert this.mode == 0;
        _n_Client_writeFrame(path);
    }

    public void clientWriteAndSendCustomFrame(String path, byte[] bs) {
        assert this.mode == 0;
        _n_Client_writeAndSendCustom(path, bs);
    }

    public void clientWriteCustomFrame(String path, byte[] bs) {
        assert this.mode == 0;
        _n_Client_writeCustomFrame(path, bs);
    }

    //========SERVER=============
    //
    public void serverShowFrameBlocking() {
        assert this.mode == 1;
        _n_Server_showFrame_blocking();
    }

    public byte[] serverRetrieveFrame() {
        assert this.mode == 1;
        return _n_Server_retrieveFrame();
    }

    public void serverWriteReceivedFrame(String path) {
        assert this.mode == 1;
        _n_Server_writeRetrivedFrame(path);
    }

    public void serverWriteAndShowReceivedFrame(String path) {
        assert this.mode == 1;
        _n_Server_showAndWriteFrame(path);
    }
}
