#include "Hardware.h"
#include <Arduino.h>

Hardware::Hardware() {
  for (uint8_t i = 0; i < 16; i++) {
    lastButtonStates[i] = HIGH;  // Pull-up default
    lastButtonChange[i] = 0;
  }

  for (uint8_t i = 0; i < 4; i++) {
    lastPotValues[i] = 0;
    lastSliderValues[i] = 0;
    potSmoothed[i] = 0.0;
    sliderSmoothed[i] = 0.0;
  }
}

void Hardware::init() {
  // Initialize button pins with pull-up resistors
  for (uint8_t i = 0; i < 16; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Initialize pot pins
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(potPins[i], INPUT);
    uint16_t raw = analogRead(potPins[i]);
    lastPotValues[i] = raw >> 3;  // Initial read
    potSmoothed[i] = (float)raw;  // Initialize smoothing buffer
  }

  // Initialize slider pins
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(sliderPins[i], INPUT);
    uint16_t raw = analogRead(sliderPins[i]);
    lastSliderValues[i] = raw >> 3;  // Initial read
    sliderSmoothed[i] = (float)raw;  // Initialize smoothing buffer
  }

  // Initialize LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

bool Hardware::readButtonPress(uint8_t index) {
  if (index >= 16) return false;

  unsigned long currentTime = millis();

  // Read current state
  bool currentState = digitalRead(buttonPins[index]);

  // Check if state changed
  if (currentState != lastButtonStates[index]) {
    // Debounce check
    if (currentTime - lastButtonChange[index] > DEBOUNCE_MS) {
      lastButtonChange[index] = currentTime;
      lastButtonStates[index] = currentState;

      // Return true on button press (transition to LOW)
      return (currentState == LOW);
    }
  }

  return false;
}

bool Hardware::readButtonState(uint8_t index) {
  if (index >= 16) return false;
  return digitalRead(buttonPins[index]) == LOW;
}

uint8_t Hardware::readPot(uint8_t index) {
  if (index >= 4) return 0;

  // Read 10-bit ADC (0-1023)
  uint16_t raw = analogRead(potPins[index]);

  // Apply exponential moving average smoothing
  // smoothed = (alpha * new) + ((1 - alpha) * smoothed)
  potSmoothed[index] = (SMOOTHING_FACTOR * raw) + ((1.0 - SMOOTHING_FACTOR) * potSmoothed[index]);

  // Scale to 7-bit MIDI (0-127)
  uint8_t value = ((uint16_t)potSmoothed[index]) >> 3;

  lastPotValues[index] = value;
  return value;
}

int16_t Hardware::readPotChange(uint8_t index, uint8_t threshold) {
  if (index >= 4) return -1;

  // Save old value BEFORE reading new one
  uint8_t oldValue = lastPotValues[index];

  // Read current pot value (0-1023) and scale to 7-bit MIDI (0-127)
  uint16_t raw = analogRead(potPins[index]);
  uint8_t newValue = raw >> 3;  // Divide by 8

  // Check if change exceeds threshold
  if (abs((int16_t)newValue - (int16_t)oldValue) >= threshold) {
    lastPotValues[index] = newValue;
    return newValue;
  }

  return -1;  // No significant change
}

void Hardware::setLED(bool state) {
  digitalWrite(ledPin, state ? HIGH : LOW);
}

void Hardware::setLEDBrightness(uint8_t brightness) {
  analogWrite(ledPin, brightness);
}

void Hardware::toggleLED() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

uint8_t Hardware::getLastPotValue(uint8_t index) const {
  if (index >= 4) return 0;
  return lastPotValues[index];
}

uint8_t Hardware::readSlider(uint8_t index) {
  if (index >= 4) return 0;

  // Read 10-bit ADC (0-1023)
  uint16_t raw = analogRead(sliderPins[index]);

  // Apply exponential moving average smoothing
  // smoothed = (alpha * new) + ((1 - alpha) * smoothed)
  sliderSmoothed[index] = (SMOOTHING_FACTOR * raw) + ((1.0 - SMOOTHING_FACTOR) * sliderSmoothed[index]);

  // Scale to 7-bit MIDI (0-127)
  uint8_t value = ((uint16_t)sliderSmoothed[index]) >> 3;

  lastSliderValues[index] = value;
  return value;
}

int16_t Hardware::readSliderChange(uint8_t index, uint8_t threshold) {
  if (index >= 4) return -1;

  // Save old value BEFORE reading new one
  uint8_t oldValue = lastSliderValues[index];

  // Read current slider value (0-1023) and scale to 7-bit MIDI (0-127)
  uint16_t raw = analogRead(sliderPins[index]);
  uint8_t newValue = raw >> 3;  // Divide by 8

  // Check if change exceeds threshold
  if (abs((int16_t)newValue - (int16_t)oldValue) >= threshold) {
    lastSliderValues[index] = newValue;
    return newValue;
  }

  return -1;  // No significant change
}

uint8_t Hardware::getLastSliderValue(uint8_t index) const {
  if (index >= 4) return 0;
  return lastSliderValues[index];
}

InputState Hardware::getCurrentState() {
  InputState state;

  // Read all pots
  for (uint8_t i = 0; i < 4; i++) {
    state.pots[i] = readPot(i);
  }

  // Read all sliders
  for (uint8_t i = 0; i < 4; i++) {
    state.sliders[i] = readSlider(i);
  }

  // Read all button states (current, not debounced)
  for (uint8_t i = 0; i < 16; i++) {
    state.buttons[i] = readButtonState(i);
  }

  return state;
}
