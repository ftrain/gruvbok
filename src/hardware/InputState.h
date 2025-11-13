#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <stdint.h>

/**
 * InputState - Snapshot of all hardware inputs
 *
 * Clean separation: Hardware reads physical inputs,
 * returns this struct with no logic or interpretation.
 */
struct InputState {
  uint8_t pots[4];      // Pot values (0-127)
  uint8_t sliders[4];   // Slider values (0-127)
  bool buttons[16];     // Button states (true = pressed)

  InputState() {
    for (uint8_t i = 0; i < 4; i++) {
      pots[i] = 0;
      sliders[i] = 0;
    }
    for (uint8_t i = 0; i < 16; i++) {
      buttons[i] = false;
    }
  }
};

#endif // INPUTSTATE_H
