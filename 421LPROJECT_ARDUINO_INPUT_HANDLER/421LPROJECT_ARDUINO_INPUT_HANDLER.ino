#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PINS ---
const int redLed = 13;
const int blueLed = 12;
const int greenLed = 11;
const int yellowLed = 10;
const int buzzer = 7;

const int redBtn = 2;
const int blueBtn = 3;
const int greenBtn = 4;
const int yellowBtn = 5;

// --- CODES ---
const byte CODERED    = 0x01;
const byte CODEBLUE   = 0x02;
const byte CODEGREEN  = 0x04;
const byte CODEYELLOW = 0x08;
const byte CODE_CORRECT = 0x10;
const byte CODE_WRONG   = 0x20;
const byte CODE_NONE    = 0x00;
const byte CODE_START   = 0x55;

// --- DISTINCT FREQUENCIES (Octaves for maximum separation) ---
const int TONE_RED    = 220;  // A3 (Low Rumble)
const int TONE_BLUE   = 440;  // A4 (Standard Mid)
const int TONE_GREEN  = 880;  // A5 (High)
const int TONE_YELLOW = 1760; // A6 (Very High piercing)

// --- STATE ---
bool isGameActive = false;
int localScore = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // XBee on Pins 18/19

  lcd.init();
  lcd.backlight();
  showStartScreen();

  // LEDs (Active LOW)
  pinMode(redLed, OUTPUT); digitalWrite(redLed, HIGH); 
  pinMode(blueLed, OUTPUT); digitalWrite(blueLed, HIGH);
  pinMode(greenLed, OUTPUT); digitalWrite(greenLed, HIGH);
  pinMode(yellowLed, OUTPUT); digitalWrite(yellowLed, HIGH);

  pinMode(buzzer, OUTPUT);

  pinMode(redBtn, INPUT_PULLUP);
  pinMode(blueBtn, INPUT_PULLUP);
  pinMode(greenBtn, INPUT_PULLUP);
  pinMode(yellowBtn, INPUT_PULLUP);
}

void loop() {
  // 1. LISTEN
  if (Serial1.available() > 0) {
    handleIncomingCommand(Serial1.read());
  }

  // 2. SPEAK
  checkButton(redBtn, CODERED, "Red", TONE_RED);
  checkButton(blueBtn, CODEBLUE, "Blue", TONE_BLUE);
  checkButton(greenBtn, CODEGREEN, "Green", TONE_GREEN);
  checkButton(yellowBtn, CODEYELLOW, "Yellow", TONE_YELLOW);
}

void handleIncomingCommand(byte cmd) {
  if (cmd == CODE_NONE) { 
    turnOffAllLeds(); 
    noTone(buzzer); // Stop sound when light goes off
    return; 
  }

  turnOffAllLeds();

  if (!isGameActive && cmd != CODE_WRONG) {
      isGameActive = true;
      lcd.clear(); lcd.print("Game On!");
  }

  switch (cmd) {
    // Added tone() here so you hear the pattern when PC plays it
    case CODERED:    
      flashLed(redLed, "Watch: RED"); 
      tone(buzzer, TONE_RED); 
      break;
    case CODEBLUE:   
      flashLed(blueLed, "Watch: BLUE"); 
      tone(buzzer, TONE_BLUE); 
      break;
    case CODEGREEN:  
      flashLed(greenLed, "Watch: GREEN"); 
      tone(buzzer, TONE_GREEN); 
      break;
    case CODEYELLOW: 
      flashLed(yellowLed, "Watch: YELLOW"); 
      tone(buzzer, TONE_YELLOW); 
      break;

    case CODE_CORRECT:
      localScore++;
      lcd.clear(); lcd.print("CORRECT!");
      lcd.setCursor(0,1); lcd.print("Score: "); lcd.print(localScore);
      playCorrectSound();
      break;

    case CODE_WRONG:
      lcd.clear(); lcd.print("GAME OVER");
      lcd.setCursor(0,1); lcd.print("Score: "); lcd.print(localScore);
      playWrongSound();
      isGameActive = false; 
      localScore = 0;
      delay(2000);
      showStartScreen();
      break;
  }
}

void checkButton(int pin, byte colorCode, const char* name, int freq) {
  if (digitalRead(pin) == LOW) {
    if (!isGameActive) {
      // Start Signal
      Serial1.write(CODE_START);
      isGameActive = true;
      lcd.clear(); lcd.print("Starting...");
      playStartSound();
    } else {
      // Game Move
      Serial1.write(colorCode);
      lcd.clear(); lcd.print(name);
      
      // Turn ON LED (Low)
      if(pin==redBtn) digitalWrite(redLed, LOW);
      if(pin==blueBtn) digitalWrite(blueLed, LOW);
      if(pin==greenBtn) digitalWrite(greenLed, LOW);
      if(pin==yellowBtn) digitalWrite(yellowLed, LOW);
      
      tone(buzzer, freq);
    }

    while(digitalRead(pin) == LOW) delay(10); // Wait for release

    turnOffAllLeds();
    noTone(buzzer);
    delay(50);
  }
}

void showStartScreen() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("PRESS ANY BTN");
  lcd.setCursor(0,1); lcd.print("TO START GAME");
}

void flashLed(int pin, const char* text) {
  digitalWrite(pin, LOW); // ON
  lcd.clear(); lcd.print(text);
}

void turnOffAllLeds() {
  digitalWrite(redLed, HIGH); // OFF
  digitalWrite(blueLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, HIGH);
}

// --- SOUND EFFECTS ---

void playCorrectSound() {
  // Fast "Coin Collect" sound
  tone(buzzer, 1500, 50); delay(50);
  tone(buzzer, 2000, 50); delay(50);
  tone(buzzer, 2500, 100); delay(100);
  noTone(buzzer);
}

void playWrongSound() {
  // Slow "Error" Buzz
  tone(buzzer, 150, 300); delay(300);
  tone(buzzer, 100, 500); delay(500);
  noTone(buzzer);
}

void playStartSound() {
  // "Power Up" Sweep
  tone(buzzer, TONE_RED, 100); delay(100);
  tone(buzzer, TONE_BLUE, 100); delay(100);
  tone(buzzer, TONE_GREEN, 100); delay(100);
  tone(buzzer, TONE_YELLOW, 300); delay(300);
  noTone(buzzer);
}