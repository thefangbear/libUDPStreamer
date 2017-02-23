package in.derros.jni;

// UDPStreamer JNI Interface
// @author R-J Alexander Fang
//

public class UDPStreamer {

	private native byte[] _n_grabFrame();
	private native void   _n_sendFrame();
	private native void   _n_showFrame_blocking();
	public UDPStreamer(String lp) { System.load(lp); }

	public byte[] grabFrame() {
		return _n_grabFrame();
	}

	public void sendFrame() { _n_sendFrame(); }

	public void showFrameBlocking() { _n_showFrame_blocking(); }

	public static void main(String[] args) {
		UDPStreamer streamer = new UDPStreamer("/Users/derros/Projects/udp-image-streaming/build/libUDPStreamer.dylib");
		byte[] bs = streamer.grabFrame();
		for(byte b : bs) { System.out.println("Byte received: " + b); }
	}
}
