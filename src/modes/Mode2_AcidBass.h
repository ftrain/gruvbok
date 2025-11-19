#ifndef MODE2_ACIDBASS_H
#define MODE2_ACIDBASS_H

#include "Mode.h"

/**
 * Mode2 - Acid Bass Synthesizer
 *
 * A minimalistic monophonic acid bass sequencer with slide and accent.
 * Classic TB-303 inspired design for deep, squelchy basslines.
 *
 * Track mapping:
 * - 8 tracks available for acid sequences
 * - Each track is a monophonic voice (typically use 1-2 tracks)
 *
 * Event interpretation:
 * - Switch: Trigger note
 * - Pot 0: Pitch (0-127, maps to bass range C1-C4, 36-72)
 * - Pot 1: Accent (0-127, adds velocity boost 0-63)
 * - Pot 2: Note length (0-127, maps to 10ms-2000ms gate time)
 * - Pot 3: Slide (0-127, portamento amount)
 *
 * MIDI output:
 * - Notes in bass range (C1-C4)
 * - Velocity varies with accent
 * - CC 5 (Portamento Time) for slide amount
 * - CC 65 (Portamento On/Off) for slide enable
 *
 * Typical use:
 * - Base velocity: 80
 * - Accent adds up to +47 velocity (max 127)
 * - Slide creates glide between notes
 * - Short gates for plucky sound, long for sustained
 */
class Mode2_AcidBass : public Mode {
private:
  // Bass note range: C1 (36) to C4 (72)
  static constexpr uint8_t MIN_NOTE = 36;  // C1
  static constexpr uint8_t MAX_NOTE = 72;  // C4
  static constexpr uint8_t NOTE_RANGE = MAX_NOTE - MIN_NOTE;

  // Base velocity and accent range
  static constexpr uint8_t BASE_VELOCITY = 80;
  static constexpr uint8_t MAX_ACCENT = 47;  // Accent adds 0-47 (max 127)

  // Last note for slide detection (per track)
  mutable uint8_t lastNote[8];

public:
  Mode2_AcidBass(uint8_t channel) : Mode(channel) {
    // Initialize last notes
    for (uint8_t i = 0; i < 8; i++) {
      lastNote[i] = 0;
    }
  }

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (trackIndex >= 8) return;

    // Only process if switch is active
    if (!event.getSwitch()) {
      lastNote[trackIndex] = 0;  // Reset last note
      return;
    }

    // Read parameters from stored event pots
    uint8_t pitchValue = event.getPot(0);    // Pot 0: Pitch (0-127)
    uint8_t accentValue = event.getPot(1);   // Pot 1: Accent (0-127)
    uint8_t lengthValue = event.getPot(2);   // Pot 2: Note length (0-127)
    uint8_t slideValue = event.getPot(3);    // Pot 3: Slide (0-127)

    // Map pitch to bass note range (C1-C4, MIDI 36-72)
    uint8_t note = MIN_NOTE + ((pitchValue * NOTE_RANGE) / 127);
    if (note > MAX_NOTE) note = MAX_NOTE;

    // Calculate velocity with accent
    uint8_t accent = (accentValue * MAX_ACCENT) / 127;
    uint8_t velocity = BASE_VELOCITY + accent;
    if (velocity > 127) velocity = 127;

    // Map length value (0-127) to note duration (10ms - 2000ms)
    unsigned long noteLength = 10 + ((unsigned long)lengthValue * 1990) / 127;

    // Slide/portamento: if slide > 0 and we have a previous note
    bool hasSlide = (slideValue > 0 && lastNote[trackIndex] > 0);

    if (hasSlide) {
      // Map slide value to portamento time (0-127)
      // Higher values = slower slide
      uint8_t portamentoTime = slideValue;

      // Enable portamento
      output.cc(midiChannel, 65, 127, 0);  // CC65: Portamento On/Off (127=on)
      output.cc(midiChannel, 5, portamentoTime, 0);  // CC5: Portamento Time
    } else {
      // Disable portamento for non-slide notes
      output.cc(midiChannel, 65, 0, 0);  // CC65: Portamento Off
    }

    // Send note on/off
    output.noteOn(midiChannel, note, velocity, 0);
    output.noteOff(midiChannel, note, noteLength);

    // Remember this note for next slide detection
    lastNote[trackIndex] = note;

    // Unused parameter
    (void)stepTime;
  }

  const char* getName() const override {
    return "AcidBass";
  }
};

#endif // MODE2_ACIDBASS_H
