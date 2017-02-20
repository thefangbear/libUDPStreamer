
class UDPStreamerJNI {
    private native void native_blocking_receive();
    private native void native_send();

    public UDPStreamerJNI() { System.loadLibrary("libjni_udp_streaming_lib"); }

    public void sendOnce() { native_send(); }

    public void sendBlocking() {
        while(true) { sendOnce(); }
    }

    public void displayReceived() { native_blocking_receive(); }
}
