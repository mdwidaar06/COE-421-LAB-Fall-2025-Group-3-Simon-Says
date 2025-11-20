public class Main {
    public static void main(String[] args) {
        // REPLACE "COM12" with your actual XBee Explorer COM port
        String portName = "COM8"; 
        
        try {
            SerialPortHandle port = new SerialPortHandle(portName);
            MemoryGame game = new MemoryGame(port);
            game.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}