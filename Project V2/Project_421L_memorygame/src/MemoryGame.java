import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MemoryGame {

    // --- Protocol Constants (Must match Arduino) ---
    private static final byte RED    = 0x01;
    private static final byte BLUE   = 0x02;
    private static final byte GREEN  = 0x04;
    private static final byte YELLOW = 0x08;

    private static final byte CORRECT = 0x10;
    private static final byte WRONG   = 0x20;
    private static final byte NONE    = 0x00;
   
    // The Magic Byte to start the game
    private static final byte START   = 0x55;

    private SerialPortHandle port;

    public MemoryGame(SerialPortHandle port) {
        this.port = port;
    }

    public void start() throws Exception {
        // OUTER LOOP: Allows the game to restart after "Game Over"
        while (true) {
            System.out.println("\n==========================================");
            System.out.println("      WAITING FOR START SIGNAL...         ");
            System.out.println("  (Press ANY button on Arduino to Start)  ");
            System.out.println("==========================================");

            // 1. Clear any old garbage data so we don't start accidentally
            port.clearBuffer();

            // 2. Blocking Wait for START (0x55) from Arduino
            // The Arduino code I gave you earlier sends 0x55 if you press a button while the game is idle.
            while (true) {
                byte input = port.readByte();
                if (input == START) {
                    System.out.println(">> START SIGNAL RECEIVED! GAME ON! <<");
                    // Brief pause to let the start sound finish playing on Arduino
                    Thread.sleep(1500);
                    break;
                }
            }

            // --- Start the Game Session ---
            playSession();
           
            // After Game Over, loop back to top and wait for start again
            System.out.println("Reseting system...");
            Thread.sleep(2000);
        }
    }

    private void playSession() throws Exception {
        List<Byte> pattern = new ArrayList<>();
        Random rand = new Random();
       
        int score = 0;
        boolean playing = true;

        while (playing) {
            // Generate new color
            byte nextColor = (byte) (1 << rand.nextInt(4));
            pattern.add(nextColor);

            System.out.println("\n--- Level " + (score + 1) + " ---");
            System.out.println("Displaying pattern...");

            // Show Pattern
            Thread.sleep(500);
            for (byte b : pattern) {
                port.writeByte(b);      // Send Color (Arduino plays Tone + Lights LED)
                Thread.sleep(600);      // Hold for 600ms
                port.writeByte(NONE);   // Turn off
                Thread.sleep(250);      // Gap between notes
            }

            // Receive Input
            System.out.println("Waiting for player input...");
            boolean correct = true;

            for (byte expected : pattern) {
                byte userByte = port.readByte(); // Wait for XBee
                System.out.print("Received: " + getColorName(userByte));

                if (userByte != expected) {
                    System.out.println(" -> WRONG!");
                    correct = false;
                    break;
                } else {
                    System.out.println(" -> OK");
                }
            }

            // Send Result
            if (correct) {
                score++;
                System.out.println("Round Cleared! Score: " + score);
                port.writeByte(CORRECT); // Arduino plays "Success" jingle
            } else {
                System.out.println("Game Over. Final Score: " + score);
                port.writeByte(WRONG);   // Arduino plays "Fail" buzz
                playing = false;
            }

            Thread.sleep(1500); // Wait before next level
        }
    }
   
    private String getColorName(byte b) {
        switch(b) {
            case RED: return "RED";
            case BLUE: return "BLUE";
            case GREEN: return "GREEN";
            case YELLOW: return "YELLOW";
            case START: return "START BUTTON";
            default: return "UNKNOWN (" + String.format("0x%02X", b) + ")";
        }
    }
}