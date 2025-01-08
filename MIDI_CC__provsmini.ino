// Outputs MIDI control messages based on the position of a joystick
// Controls the detune of 4 oscillators via continuous control messages
// in a Behringer ProVS Mini synthesizer
// Osc A: cc 111 (left)
// Osc B: cc 112 (up)
// Osc C: cc 113 (right)
// Osc D: cc 114 (down)

// The DIN port viewed from the back (not the side the cable comes from):
//      
//  1       3
//   4     5
//      2
// pin 1 = not connected
// pin 2 = connected to ground
// pin 3 = not connected
// pin 4 = connected to 220 ohm resistor, then to +5V
// pin 5 = connected to 220 ohm resistor, then to TX2 (pin 17) of the ESP32


#include <Arduino.h>

#define LED_BUILTIN 2
#define POT_X 32
#define POT_Y 33
#define BUTTON 25

void noteOn(int pitch, int velocity) {
  // Turns
  Serial2.write(144);    // 0x90 = 144  this is the note on command
  Serial2.write(pitch);
  Serial2.write(velocity);
}

void noteOff(int pitch, int velocity) {
  Serial2.write(128);    // 0x80 = 128  this is the note off command
  Serial2.write(pitch);
  Serial2.write(velocity);
}

void controlMsg(int cc_number, int value) {  
  Serial2.write(176);  // 0xB0 = 176  this is the control change command
  Serial2.write(cc_number);
  Serial2.write(value);
}

void playNote_onoff() {
  // Plays a single note for 700 ms repeatedly, for testing purposes
  // MIDI notes range from 0 - 127
  // Middle C is note 60
  int note = 60;
  noteOn(note, 100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(700);

  noteOff(note, 0);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
}


void setup() {
  // Set MIDI baud rate
  // "Serial2" is the TX2 output of the ESP32
  // Connect this to pin 4 of the DIN jack
  Serial2.begin(31250);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(POT_X, INPUT);
  pinMode(POT_Y, INPUT);
}

void loop() {
  // read the joystick x and y pots
  int x_value = analogRead(POT_X);
  int y_value = analogRead(POT_Y);
  
  // check the position of the joystick
  // if the joystick is in the left half, control osc A, etc.
  if (x_value < 2048) {
    // osc A ==> cc 111
    int x_map = map(x_value, 0, 2047, 0, 99);
    controlMsg(111, x_map);
  }
  else {
    // osc C ==> cc 113
    int x_map = map(x_value, 2048, 4095, 99, 0);
    controlMsg(113, x_map);
  }

  if (y_value < 2048) {
    // osc B ==> cc 112
    int y_map = map(y_value, 0, 2047, 0, 99);
    controlMsg(112, y_map);
  }
  else {
    // osc D ==> cc 114
    int y_map = map(y_value, 2048, 4095, 99, 0);
    controlMsg(114, y_map);
  }
  
  // playNote_onoff();
}


