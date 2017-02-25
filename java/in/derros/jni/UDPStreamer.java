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
        _n_init_client(serverAddress, serverPort, cameraNumber, encodeQuality, frameWidth, frameHeight);
    }

    public UDPStreamer(String lp, short portOfThisServer) {
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


    //=======TESTER=============
    //
    public static void main(String[] args) {
        UDPStreamer streamer =
                new UDPStreamer(
                        "/home/alex/Projects/test-img-streaming/client/build/libUDPStreamer.so",
                        "client");
        // TODO test
    }

}
