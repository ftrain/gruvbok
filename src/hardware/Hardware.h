#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include "InputState.h"

/**
 * Hardware - Abstraction layer for GRUVBOK hardware I/O
 *
 * Manages:
 * - 16 momentary buttons (B1-B16)
 * - 4 slider potentiometers (S1-S4)
 * - LED output
 * - Debouncing and state tracking
 */
class Hardware {
private:
  // Pin definitions
  static constexpr uint8_t buttonPins[16] = {
    23, 22, 21, 20, 19, 18, 17, 16,  // B1-B8
    9, 8, 7, 6, 5, 4, 3, 2            // B9-B16
  };

  static constexpr uint8_t potPins[4] = {
    24, 25, 26, 27  // Pots 1-4: Tempo, Mode, Pattern, Track
  };

  static constexpr uint8_t sliderPins[4] = {
    41, 40, 39, 38  // Pots 5-8 (sliders): Velocity, Flam, Length, Pan
  };

  static constexpr uint8_t ledPin = 13;

  // State tracking
  bool lastButtonStates[16];
  uint8_t lastPotValues[4];
  uint8_t lastSliderValues[4];

  // Smoothing (exponential moving average)
  float potSmoothed[4];      // Smoothed pot values (float for precision)
  float sliderSmoothed[4];   // Smoothed slider values
  static constexpr float SMOOTHING_FACTOR = 0.3;  // 0.0=no smoothing, 1.0=no filtering

  // Debouncing
  unsigned long lastButtonChange[16];
  static constexpr unsigned long DEBOUNCE_MS = 20;

public:
  Hardware();

  /**
   * Initialize hardware pins
   */
  void init();

  /**
   * Read button state (returns true on press, false otherwise)
   * Handles debouncing internally
   * @param index Button index (0-15)
   * @return true if button was just pressed
   */
  bool readButtonPress(uint8_t index);

  /**
   * Read button current state (not debounced)
   * @param index Button index (0-15)
   * @return true if button is currently pressed
   */
  bool readButtonState(uint8_t index);

  /**
   * Read potentiometer value (0-127)
   * @param index Pot index (0-3)
   * @return MIDI-scaled value (0-127)
   */
  uint8_t readPot(uint8_t index);

  /**
   * Read potentiometer with change detection
   * @param index Pot index (0-3)
   * @param threshold Minimum change to register (default 2)
   * @return Pot value, or -1 if no significant change
   */
  int16_t readPotChange(uint8_t index, uint8_t threshold = 2);

  /**
   * Set LED state
   * @param state true=on, false=off
   */
  void setLED(bool state);

  /**
   * Set LED brightness (PWM)
   * @param brightness 0-255 (0=off, 255=full)
   */
  void setLEDBrightness(uint8_t brightness);

  /**
   * Toggle LED state
   */
  void toggleLED();

  /**
   * Get last pot value without reading hardware
   */
  uint8_t getLastPotValue(uint8_t index) const;

  /**
   * Read slider value (0-127)
   * @param index Slider index (0-3)
   * @return MIDI-scaled value (0-127)
   */
  uint8_t readSlider(uint8_t index);

  /**
   * Read slider with change detection
   * @param index Slider index (0-3)
   * @param threshold Minimum change to register (default 2)
   * @return Slider value, or -1 if no significant change
   */
  int16_t readSliderChange(uint8_t index, uint8_t threshold = 2);

  /**
   * Get last slider value without reading hardware
   */
  uint8_t getLastSliderValue(uint8_t index) const;

  /**
   * Get current state snapshot of all inputs
   * Pure dataflow: Hardware → InputState (no logic)
   */
  InputState getCurrentState();
};

#endif // HARDWARE_H
