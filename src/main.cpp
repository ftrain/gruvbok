#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>


// --- PIN SETTINGS ---
const int buttonPin1 = 23;  // Digital Pins for Buttons
const int buttonPin2 = 22;
const int buttonPin3 = 21;     
const int buttonPin4 = 20;
const int buttonPin5 = 19;
const int buttonPin6 = 18;  
const int buttonPin7 = 17;
const int buttonPin8 = 16;

const int buttonPin9 = 9;  
const int buttonPin10 = 8;
const int buttonPin11 = 7;
const int buttonPin12 = 6;  
const int buttonPin13 = 5;
const int buttonPin14 = 4;
const int buttonPin15 = 3;  
const int buttonPin16 = 2; 
 
const int potPin1    = 24;  // Pins for potentiometer input
const int potPin2    = 25;
const int potPin3    = 26;    
const int potPin4    = 27;

const int potPin5    = 41;
const int potPin6    = 40; 
const int potPin7    = 39;
const int potPin8    = 38;

const int midiChannel = 1;   // MIDI channel (1-16)

// --- STATE TRACKING ---
int lastButtonState1 = HIGH;  // assume pullup
int lastButtonState2 = HIGH;
int lastButtonState3 = HIGH; 
int lastButtonState4 = HIGH;
int lastButtonState5 = HIGH; 
int lastButtonState6 = HIGH;
int lastButtonState7 = HIGH; 
int lastButtonState8 = HIGH;
int lastButtonState9 = HIGH; 
int lastButtonState10 = HIGH;
int lastButtonState11 = HIGH; 
int lastButtonState12 = HIGH;
int lastButtonState13 = HIGH; 
int lastButtonState14 = HIGH;
int lastButtonState15 = HIGH; 
int lastButtonState16 = HIGH; 


int lastCCValue1 = -1;
int lastCCValue2 = -1;
int lastCCValue3 = -1;
int lastCCValue4 = -1;
int lastCCValue5 = -1;
int lastCCValue6 = -1;
int lastCCValue7 = -1;
int lastCCValue8 = -1;

void CheckHardware(){

  // --- Handle Button 1 as MIDI Note ---
  int buttonState1 = digitalRead(buttonPin1);
  if (buttonState1 != lastButtonState1) {        //Something changed!
    if (buttonState1 == LOW) {
      usbMIDI.sendNoteOn(60, 100, midiChannel);  // Note on (Middle C)
    } else {
      usbMIDI.sendNoteOff(60, 0, midiChannel);   // Note off
    }
    lastButtonState1 = buttonState1;
  }

  // --- Handle Button 2 as MIDI Note ---
  int buttonState2 = digitalRead(buttonPin2);
  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      usbMIDI.sendNoteOn(61, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(61, 0, midiChannel);
    }
    lastButtonState2 = buttonState2;
  }

  // --- Handle Button 3 as MIDI Note ---
  int buttonState3 = digitalRead(buttonPin3);
  if (buttonState3 != lastButtonState3) {
    if (buttonState3 == LOW) {
      usbMIDI.sendNoteOn(62, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(62, 0, midiChannel);
    }
    lastButtonState3 = buttonState3;
  }

  // --- Handle Button 4 as MIDI Note ---
  int buttonState4 = digitalRead(buttonPin4);
  if (buttonState4 != lastButtonState4) {
    if (buttonState4 == LOW) {
      usbMIDI.sendNoteOn(63, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(63, 0, midiChannel);
    }
    lastButtonState4 = buttonState4;
  }

  // --- Handle Button 5 as MIDI Note ---
  int buttonState5 = digitalRead(buttonPin5);
  if (buttonState5 != lastButtonState5) {
    if (buttonState5 == LOW) {
      usbMIDI.sendNoteOn(64, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(64, 0, midiChannel);
    }
    lastButtonState5 = buttonState5;
  }

  // --- Handle Button 6 as MIDI Note ---
  int buttonState6 = digitalRead(buttonPin6);
  if (buttonState6 != lastButtonState6) {
    if (buttonState6 == LOW) {
      usbMIDI.sendNoteOn(65, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(65, 0, midiChannel);
    }
    lastButtonState6 = buttonState6;
  }

  // --- Handle Button 7 as MIDI Note ---
  int buttonState7 = digitalRead(buttonPin7);
  if (buttonState7 != lastButtonState7) {
    if (buttonState7 == LOW) {
      usbMIDI.sendNoteOn(66, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(66, 0, midiChannel);
    }
    lastButtonState7 = buttonState7;
  }

  // --- Handle Button 8 as MIDI Note ---
  int buttonState8 = digitalRead(buttonPin8);
  if (buttonState8 != lastButtonState8) {
    if (buttonState8 == LOW) {
      usbMIDI.sendNoteOn(67, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(67, 0, midiChannel);
    }
    lastButtonState8 = buttonState8;
  }

  // --- Handle Button 9 as MIDI Note ---
  int buttonState9 = digitalRead(buttonPin9);
  if (buttonState9 != lastButtonState9) {
    if (buttonState9 == LOW) {
      usbMIDI.sendNoteOn(68, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(68, 0, midiChannel);
    }
    lastButtonState9 = buttonState9;
  }

  // --- Handle Button 10 as MIDI Note ---
  int buttonState10 = digitalRead(buttonPin10);
  if (buttonState10 != lastButtonState10) {
    if (buttonState10 == LOW) {
      usbMIDI.sendNoteOn(69, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(69, 0, midiChannel);
    }
    lastButtonState10 = buttonState10;
  }

  // --- Handle Button 11 as MIDI Note ---
  int buttonState11 = digitalRead(buttonPin11);
  if (buttonState11 != lastButtonState11) {
    if (buttonState11 == LOW) {
      usbMIDI.sendNoteOn(70, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(70, 0, midiChannel);
    }
    lastButtonState11 = buttonState11;
  }

  // --- Handle Button 12 as MIDI Note ---
  int buttonState12 = digitalRead(buttonPin12);
  if (buttonState12 != lastButtonState12) {
    if (buttonState12 == LOW) {
      usbMIDI.sendNoteOn(71, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(71, 0, midiChannel);
    }
    lastButtonState12 = buttonState12;
  }

  // --- Handle Button 13 as MIDI Note ---
  int buttonState13 = digitalRead(buttonPin13);
  if (buttonState13 != lastButtonState13) {
    if (buttonState13 == LOW) {
      usbMIDI.sendNoteOn(72, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(72, 0, midiChannel);
    }
    lastButtonState13 = buttonState13;
  }

  // --- Handle Button 14 as MIDI Note ---
  int buttonState14 = digitalRead(buttonPin14);
  if (buttonState14 != lastButtonState14) {
    if (buttonState14 == LOW) {
      usbMIDI.sendNoteOn(73, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(73, 0, midiChannel);
    }
    lastButtonState14 = buttonState14;
  }

  // --- Handle Button 15 as MIDI Note ---
  int buttonState15 = digitalRead(buttonPin15);
  if (buttonState15 != lastButtonState15) {
    if (buttonState15 == LOW) {
      usbMIDI.sendNoteOn(74, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(74, 0, midiChannel);
    }
    lastButtonState15 = buttonState15;
  }

  // --- Handle Button 16 as MIDI Note ---
  int buttonState16 = digitalRead(buttonPin16);
  if (buttonState16 != lastButtonState16) {
    if (buttonState16 == LOW) {
      usbMIDI.sendNoteOn(75, 100, midiChannel);
    } else {
      usbMIDI.sendNoteOff(75, 0, midiChannel);
    }
    lastButtonState16 = buttonState16;
  }




  // --- Handle Potentiometer 1 as MIDI CC ---
  int potValue1 = analogRead(potPin1) / 8; // Scale 0–1023 to 0–127
  if (potValue1 != lastCCValue1) {
    usbMIDI.sendControlChange(15, potValue1, midiChannel); // CC1
    lastCCValue1 = potValue1;
  }

  // --- Handle Potentiometer 2 as MIDI CC ---
  int potValue2 = analogRead(potPin2) / 8;
  if (potValue2 != lastCCValue2) {
    usbMIDI.sendControlChange(16, potValue2, midiChannel); // CC2
    lastCCValue2 = potValue2;
  }

  // --- Handle Potentiometer 3 as MIDI CC ---
  int potValue3 = analogRead(potPin3) / 8;
  if (potValue3 != lastCCValue3) {
    usbMIDI.sendControlChange(17, potValue3, midiChannel); // CC3
    lastCCValue3 = potValue3;
  }

  // --- Handle Potentiometer 4 as MIDI CC ---
  int potValue4 = analogRead(potPin4) / 8;
  if (potValue4 != lastCCValue4) {
    usbMIDI.sendControlChange(18, potValue4, midiChannel); // CC4
    lastCCValue4 = potValue4;
  }

  // --- Handle Potentiometer 5 as MIDI CC ---
  int potValue5 = analogRead(potPin5) / 8;
  if (potValue5 != lastCCValue5) {
    usbMIDI.sendControlChange(19, potValue5, midiChannel); // CC5
    lastCCValue5 = potValue5;
  }

  // --- Handle Potentiometer 6 as MIDI CC ---
  int potValue6 = analogRead(potPin6) / 8;
  if (potValue6 != lastCCValue6) {
    usbMIDI.sendControlChange(20, potValue6, midiChannel); // CC6
    lastCCValue6 = potValue6;
  }

  // --- Handle Potentiometer 7 as MIDI CC ---
  int potValue7 = analogRead(potPin7) / 8;
  if (potValue7 != lastCCValue7) {
    usbMIDI.sendControlChange(21, potValue7, midiChannel); // CC7
    lastCCValue7 = potValue7;
  }

  // --- Handle Potentiometer 8 as MIDI CC ---
  int potValue8 = analogRead(potPin8) / 8;
  if (potValue8 != lastCCValue8) {
    usbMIDI.sendControlChange(22, potValue8, midiChannel); // CC8
    lastCCValue8 = potValue8;
  }

}


void setup() {
  pinMode(buttonPin1, INPUT_PULLUP); // Button with pullup
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP); 
  pinMode(buttonPin5, INPUT_PULLUP);
  pinMode(buttonPin6, INPUT_PULLUP);
  pinMode(buttonPin7, INPUT_PULLUP); 
  pinMode(buttonPin8, INPUT_PULLUP);
  pinMode(buttonPin9, INPUT_PULLUP); 
  pinMode(buttonPin10, INPUT_PULLUP);
  pinMode(buttonPin11, INPUT_PULLUP);
  pinMode(buttonPin12, INPUT_PULLUP); 
  pinMode(buttonPin13, INPUT_PULLUP);
  pinMode(buttonPin14, INPUT_PULLUP);
  pinMode(buttonPin15, INPUT_PULLUP); 
  pinMode(buttonPin16, INPUT_PULLUP); 

  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);
  pinMode(potPin3, INPUT);
  pinMode(potPin4, INPUT);
  pinMode(potPin5, INPUT);
  pinMode(potPin6, INPUT);
  pinMode(potPin7, INPUT);
  pinMode(potPin8, INPUT);

}


void loop() {

  CheckHardware();

  // --- Keep USB MIDI Running ---
  while (usbMIDI.read()) {
    
  }

}