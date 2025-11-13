#ifndef MODE1_DRUMMACHINE_H
#define MODE1_DRUMMACHINE_H

#include "Mode.h"

/**
 * Mode1 - Drum Machine
 *
 * A classic drum machine with 8 tracks playing GM drum sounds.
 *
 * Track mapping:
 * Track 0: Kick (C1/36)
 * Track 1: Snare (D1/38)
 * Track 2: Closed Hi-Hat (F#1/42)
 * Track 3: Open Hi-Hat (A#1/46)
 * Track 4: Low Tom (G1/43)
 * Track 5: Mid Tom (B1/47)
 * Track 6: Crash (C#2/49)
 * Track 7: Ride (D#2/51)
 *
 * Event interpretation:
 * - Switch: Trigger drum hit
 * - Pot 0: Velocity (0-127)
 * - Pot 1: Pan (0-127, sent as CC10)
 * - Pot 2: Filter/Tone (sent as CC74)
 * - Pot 3: Reverb Send (sent as CC91)
 *
 * Note length: 50ms fixed
 */
class Mode1_DrumMachine : public Mode {
private:
  static constexpr uint8_t drumNotes[8] = {
    36,  // Kick
    38,  // Snare
    42,  // Closed Hi-Hat
    46,  // Open Hi-Hat
    43,  // Low Tom
    47,  // Mid Tom
    49,  // Crash
    51   // Ride
  };

  static constexpr unsigned long NOTE_LENGTH_MS = 50;

public:
  Mode1_DrumMachine(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event, unsigned long stepTime) override {
    if (trackIndex >= 8) return;

    // Only process if switch is active
    if (!event.getSwitch()) return;

    uint8_t note = drumNotes[trackIndex];

    // Read parameters from stored event pots
    uint8_t velocity = event.getPot(0);      // Slider 0: Velocity
    uint8_t flamAmount = event.getPot(1);    // Slider 1: Flam
    uint8_t lengthValue = event.getPot(2);   // Slider 2: Length
    uint8_t pan = event.getPot(3);           // Slider 3: Pan

    // Default velocity if zero
    if (velocity == 0) velocity = 100;

    // Map length value (0-127) to note duration (10ms - 2000ms)
    // Cast to unsigned long to prevent overflow before division
    unsigned long noteLength = 10 + ((unsigned long)lengthValue * 1990) / 127;

    // Flam: Send a quieter note slightly before main note (if flam > 0)
    if (flamAmount > 0) {
      // Flam time: 5-50ms delay based on flamAmount
      unsigned long flamDelay = 5 + ((unsigned long)flamAmount * 45) / 127;
      // Flam is quieter (60% of velocity)
      uint8_t flamVelocity = (velocity * 60) / 100;

      // Send flam note immediately
      scheduler->note(midiChannel, note, flamVelocity, 0);
      scheduler->off(midiChannel, note, noteLength / 3);  // Shorter flam note

      // Send main note after flam delay
      scheduler->note(midiChannel, note, velocity, flamDelay);
      scheduler->off(midiChannel, note, flamDelay + noteLength);
    } else {
      // No flam, just send regular note
      scheduler->note(midiChannel, note, velocity, 0);
      scheduler->off(midiChannel, note, noteLength);
    }

    // Pan: Send CC10 if pan is set
    if (pan > 0) {
      scheduler->cc(midiChannel, 10, pan, 0);  // CC10 = Pan
    }
  }

  const char* getName() const override {
    return "DrumMachine";
  }
};

#endif // MODE1_DRUMMACHINE_H
