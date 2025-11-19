#ifndef MODE4_METAARP_H
#define MODE4_METAARP_H

#include "Mode.h"

/**
 * Mode4 - Meta Arp (Directional Scale Arpeggiator)
 *
 * A unique arpeggiator that alternates direction with each active step.
 * When a step is active, it arpeggates up the scale. The next active step
 * arpeggates down. Pattern continues alternating for musical evolution.
 *
 * Track mapping:
 * - 8 independent arpeggiator tracks
 * - Each track maintains its own direction state
 *
 * Event interpretation:
 * - Switch: Trigger arpeggio
 * - Slider 0: Root note (0-127, maps to C1-C7, MIDI 24-96)
 * - Slider 1: Scale type (0-127, maps to different scales)
 * - Slider 2: Arp note duration (0-127, maps to 20ms-400ms per note)
 * - Slider 3: Number of notes (0-127, maps to 2-16 notes in arpeggio)
 *
 * Scales available:
 * - 0-15: Major (Ionian)
 * - 16-31: Minor (Aeolian)
 * - 32-47: Dorian
 * - 48-63: Phrygian
 * - 64-79: Mixolydian
 * - 80-95: Pentatonic Major
 * - 96-111: Pentatonic Minor
 * - 112-127: Chromatic
 *
 * Behavior:
 * - First active step: Arpeggio ascends
 * - Second active step: Arpeggio descends
 * - Pattern continues alternating
 * - Notes evenly spaced within the step duration
 */
class Mode4_MetaArp : public Mode {
private:
  // Note range
  static constexpr uint8_t MIN_NOTE = 24;  // C1
  static constexpr uint8_t MAX_NOTE = 96;  // C7
  static constexpr uint8_t NOTE_RANGE = MAX_NOTE - MIN_NOTE;

  // Arpeggio parameters
  static constexpr uint8_t MIN_NOTES = 2;
  static constexpr uint8_t MAX_NOTES = 16;
  static constexpr unsigned long MIN_NOTE_DURATION = 20;   // ms
  static constexpr unsigned long MAX_NOTE_DURATION = 400;  // ms
  static constexpr uint8_t BASE_VELOCITY = 100;

  // Direction tracking per track (true = up, false = down)
  mutable bool direction[8];

  // Scale intervals (semitones from root)
  static constexpr uint8_t SCALE_MAJOR[7] = {0, 2, 4, 5, 7, 9, 11};
  static constexpr uint8_t SCALE_MINOR[7] = {0, 2, 3, 5, 7, 8, 10};
  static constexpr uint8_t SCALE_DORIAN[7] = {0, 2, 3, 5, 7, 9, 10};
  static constexpr uint8_t SCALE_PHRYGIAN[7] = {0, 1, 3, 5, 7, 8, 10};
  static constexpr uint8_t SCALE_MIXOLYDIAN[7] = {0, 2, 4, 5, 7, 9, 10};
  static constexpr uint8_t SCALE_PENTATONIC_MAJOR[5] = {0, 2, 4, 7, 9};
  static constexpr uint8_t SCALE_PENTATONIC_MINOR[5] = {0, 3, 5, 7, 10};
  static constexpr uint8_t SCALE_CHROMATIC[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

public:
  Mode4_MetaArp(uint8_t channel) : Mode(channel) {
    // Initialize all tracks to upward direction
    for (uint8_t i = 0; i < 8; i++) {
      direction[i] = true;  // Start going up
    }
  }

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (trackIndex >= 8) return;

    // Only process if switch is active
    if (!event.getSwitch()) return;

    // Read parameters from stored event sliders
    uint8_t rootValue = event.getPot(0);       // Slider 0: Root note
    uint8_t scaleValue = event.getPot(1);      // Slider 1: Scale type
    uint8_t durationValue = event.getPot(2);   // Slider 2: Note duration
    uint8_t countValue = event.getPot(3);      // Slider 3: Number of notes

    // Map root to note range (C1-C7, MIDI 24-96)
    uint8_t rootNote = MIN_NOTE + ((rootValue * NOTE_RANGE) / 127);
    if (rootNote > MAX_NOTE) rootNote = MAX_NOTE;

    // Map duration (20ms-400ms)
    unsigned long noteDuration = MIN_NOTE_DURATION +
                                ((unsigned long)durationValue * (MAX_NOTE_DURATION - MIN_NOTE_DURATION)) / 127;

    // Map count (2-16 notes)
    uint8_t numNotes = MIN_NOTES + ((countValue * (MAX_NOTES - MIN_NOTES)) / 127);
    if (numNotes < MIN_NOTES) numNotes = MIN_NOTES;
    if (numNotes > MAX_NOTES) numNotes = MAX_NOTES;

    // Select scale based on scaleValue
    const uint8_t* scale;
    uint8_t scaleLength;

    if (scaleValue < 16) {
      scale = SCALE_MAJOR;
      scaleLength = 7;
    } else if (scaleValue < 32) {
      scale = SCALE_MINOR;
      scaleLength = 7;
    } else if (scaleValue < 48) {
      scale = SCALE_DORIAN;
      scaleLength = 7;
    } else if (scaleValue < 64) {
      scale = SCALE_PHRYGIAN;
      scaleLength = 7;
    } else if (scaleValue < 80) {
      scale = SCALE_MIXOLYDIAN;
      scaleLength = 7;
    } else if (scaleValue < 96) {
      scale = SCALE_PENTATONIC_MAJOR;
      scaleLength = 5;
    } else if (scaleValue < 112) {
      scale = SCALE_PENTATONIC_MINOR;
      scaleLength = 5;
    } else {
      scale = SCALE_CHROMATIC;
      scaleLength = 12;
    }

    // Generate arpeggio in current direction
    bool goingUp = direction[trackIndex];
    unsigned long delay = 0;

    for (uint8_t i = 0; i < numNotes; i++) {
      // Calculate scale degree based on direction
      uint8_t scaleDegree;
      if (goingUp) {
        scaleDegree = i % (scaleLength * 3);  // Allow up to 3 octaves
      } else {
        scaleDegree = (numNotes - 1 - i) % (scaleLength * 3);
      }

      // Get octave and scale position
      uint8_t octave = scaleDegree / scaleLength;
      uint8_t position = scaleDegree % scaleLength;

      // Calculate MIDI note
      int16_t note = rootNote + (octave * 12) + scale[position];

      // Clamp to valid MIDI range
      if (note < 0) note = 0;
      if (note > 127) note = 127;

      // Calculate velocity (slightly fade over the arpeggio)
      uint8_t velocity = BASE_VELOCITY - (i * 5);
      if (velocity < 60) velocity = 60;

      // Schedule note on/off
      output.noteOn(midiChannel, (uint8_t)note, velocity, delay);
      output.noteOff(midiChannel, (uint8_t)note, delay + noteDuration);

      // Advance delay for next note
      delay += noteDuration;
    }

    // Toggle direction for next time this track has an active step
    direction[trackIndex] = !direction[trackIndex];

    // Unused parameter
    (void)stepTime;
  }

  const char* getName() const override {
    return "MetaArp";
  }
};

#endif // MODE4_METAARP_H
