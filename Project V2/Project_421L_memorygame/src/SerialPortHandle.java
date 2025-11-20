import jssc.SerialPort;
import jssc.SerialPortException;

public class SerialPortHandle {

    private SerialPort sp;

    public SerialPortHandle(String portName) throws SerialPortException {
        sp = new SerialPort(portName);
        sp.openPort();
        sp.setParams(SerialPort.BAUDRATE_9600, 
                     SerialPort.DATABITS_8, 
                     SerialPort.STOPBITS_1, 
                     SerialPort.PARITY_NONE);
        
        // XBee Setup
        sp.setDTR(true);
        sp.setRTS(true);

        // Give the connection a moment to settle
        try { Thread.sleep(2000); } catch (InterruptedException e) {}

        clearBuffer();
    }

    // Clears old data (like button presses made before game started)
    public void clearBuffer() {
        try {
            int count = sp.getInputBufferBytesCount();
            if (count > 0) {
                sp.readBytes(count);
            }
        } catch (SerialPortException e) {
            System.err.println("Error clearing buffer: " + e.getMessage());
        }
    }

    // WAITS until a byte is received from Arduino
    public byte readByte() {
        try {
            while (sp.getInputBufferBytesCount() == 0) {
                Thread.sleep(10); // Wait 10ms and check again
            }
            byte[] buffer = sp.readBytes(1);
            return buffer[0];
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
    }

    public void writeByte(byte b) {
        try {
            sp.writeBytes(new byte[]{b});
        } catch (SerialPortException e) {
            e.printStackTrace();
        }
    }

    public void close() {
        try {
            if (sp.isOpened()) sp.closePort();
        } catch (SerialPortException e) {
            e.printStackTrace();
        }
    }
}