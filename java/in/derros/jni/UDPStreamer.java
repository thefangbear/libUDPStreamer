package in.derros.jni;

// UDPStreamer JNI Interface
// @author R-J Alexander Fang
//

public class UDPStreamer {

    //===========================INIT FUNCTION=========================


    /**
     * native _n_initialize
     * initializes UDPStreamer objects in C++
     * @param _mode 0: client 1: server
     */
    private native void _n_initialize(int _mode);

    /**
     * native _n_init_client
     * initializes a UDP streamer client
     * @param _address server address
     * @param _port server port (unsigned short)
     * @param _camNumber camera number (0 default)
     * @param _encodeQuality encoding quality, unsigned int (80 default)
     * @param _frameWidth ppi width, 1280 default
     * @param _frameHeight ppi height, 720 default
     */
    private native void _n_init_client(String _address, short _port,
                                       int _camNumber, int _encodeQuality,
                                       int _frameWidth, int _frameHeight);

    /**
     * native _n_init_server
     * initializes a UDPStreamer server
     * @param _port the port number to initialize server on (unsigned short)
     */
    private native void _n_init_server(short _port);

    /**
     * close a connection properly
     * @param _mode 0: client 1: server
     */
    private native void _n_close(int _mode);


    //
    //===========================CLIENT================================
    //

    /**
     * _n_grabFrame: Returns a camera frame for manipulation after send
     *
     * @return: byte[] the frame grabbed from camera (already sent)
     */
    private native byte[] _n_Client_grabFrame();

    /**
     * _n_Client_streamFrame: only returns a frame without sending
     *
     * @return byte[] the frame grabbed from camera (without sending)
     */
    private native byte[] _n_Client_streamFrame();

    /**
     * _n_Client_sendFrame: only sends a frame (w.o. returning data)
     */
    private native void _n_Client_sendFrame();

    /**
     * _n_sendCustomFrame: sends a custom frame
     *
     * @param bs byte[] Mat object converted to a byte
     */
    private native void _n_Client_sendCustomFrame(byte[] bs);

    //
    //===========================SERVER=================================
    //

    /**
     * _n_Server_showFrame_blocking()
     * Show frame using highgui natively implemented (multiple frame streamed together)
     */
    private native void _n_Server_showFrame_blocking();


    /**
     * _n_retrieveFrame: retrieves a frame in byte array
     *
     * @return byte[] array of bytes representing img
     */
    private native byte[] _n_Server_retrieveFrame();


    /*==========================  Java APIs ============================
     *==================================================================
    */


    private int mode; // 0 = client, 1 = server

    /**
     * Default constructor
     * @param lp String, library path
     * @param mode String, "client" or "server"
     */
    public UDPStreamer(String lp, String mode) {
        System.load(lp);
        if (mode.equals("client")) {
            this.mode = 0;
        } else {
            this.mode = 1;
        }
        _n_initialize(this.mode);
    }

    /**
     * Constructor for client
     * @param lp library path
     * @param serverAddress server address
     * @param serverPort  destination server port
     * @param cameraNumber this camera number
     * @param encodeQuality this encode quality
     * @param frameWidth ppi width
     * @param frameHeight ppi height
     */
    public UDPStreamer(String lp,
                       String serverAddress,
                       short serverPort,
                       int cameraNumber,
                       int encodeQuality,
                       int frameWidth,
                       int frameHeight) {
        _n_init_client(serverAddress, serverPort, cameraNumber, encodeQuality, frameWidth, frameHeight);
    }

    /**
     * Constructor for server
     * @param lp String library path
     * @param portOfThisServer unsigned short port to serve on
     */
    public UDPStreamer(String lp, short portOfThisServer) {
        _n_init_server(portOfThisServer);
    }

    /**
     * destructor, voluntarily called after everything's done
     */
    public void close() {
        _n_close(this.mode);
    }

    /**
     * In case close() doesn't get called, this gets called
     */
    public void finalize() {
        this.close();
    }

    // Generic method

    /**
     * Grab frame is suitable for both client and server
     * @return byte[] byte already sent or received
     */
    public byte[] genericGrabFrame() {
        if (this.mode == 0) {
            return _n_Client_grabFrame();
        } else {
            return _n_Server_retrieveFrame();
        }
    }

    //=========CLIENT===========
    //

    /**
     * Grabs a sent frame
     * @return byte[] representation of Mat
     */
    public byte[] clientGrabFrame() {
        assert this.mode == 0;
        return _n_Client_grabFrame();
    }

    /**
     * Only sends one frame without returning data
     *
     */
    public void clientSendOneFrame() {
        assert this.mode == 0;
        _n_Client_sendFrame();
    }

    /**
     * Streams frame from camera without sending to server
     * @return
     */
    public byte[] clientStreamFrame() {
        assert this.mode == 0;
        return _n_Client_streamFrame();
    }

    /**
     * Sends frame in a blocking loop to server
     *
     */
    public void clientBlockingSendFrame() {
        assert this.mode == 0;
        while (true) {
            _n_Client_sendFrame();
        }
    }

    /**
     * Sends a custom frame (uncompressed!) to client
     * @param bs byte[] representation of Mat (No compression!)
     */
    public void clientSendCustomFrame(byte[] bs) {
        assert this.mode == 0;
        _n_Client_sendCustomFrame(bs);
    }

    //========SERVER=============

    /**
     * Show every frame streamed from client in a blocking style
     */
    public void serverShowFrameBlocking() {
        assert this.mode == 1;
        _n_Server_showFrame_blocking();
    }

    /**
     * Retrieves a frame in a byte[] array
     * @return byte[] the array representation of the frame
     */
    public byte[] serverRetrieveFrame() {
        assert this.mode == 1;
        return _n_Server_retrieveFrame();
    }



}
