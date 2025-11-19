#ifndef MODE5_BASSLINEPROGRESSION_H
#define MODE5_BASSLINEPROGRESSION_H

#include "Mode.h"

/**
 * Mode5 - Bassline Progression Maker
 *
 * Generates intelligent basslines that follow chord progressions.
 * Each active step defines a chord, and the mode plays bass patterns
 * for that chord with different rhythmic variations.
 *
 * Event interpretation:
 * - Switch: Trigger bass pattern for this chord
 * - Slider 0: Root note (0-127, maps to C1-C4, MIDI 36-72)
 * - Slider 1: Scale/Chord type (0-127, different scales and modes)
 * - Slider 2: Bass pattern style (0-127, maps to different patterns)
 * - Slider 3: Note duration (0-127, maps to 50ms-1000ms)
 *
 * Bass Pattern Styles (Slider 2):
 * - 0-15: Root only (whole note)
 * - 16-31: Root + Fifth (alternating)
 * - 32-47: Root + Fifth + Octave (walking up)
 * - 48-63: Root + Third + Fifth (triad walk)
 * - 64-79: Root + Third + Fifth + Seventh (jazz walk)
 * - 80-95: Octave bounce (root low, root high)
 * - 96-111: Chromatic approach (approach from half-step below)
 * - 112-127: Fifth pedal (fifth on beats, root on offbeats)
 *
 * Scales (Slider 1):
 * - 0-15: Major
 * - 16-31: Minor
 * - 32-47: Dorian
 * - 48-63: Mixolydian
 * - 64-79: Blues
 * - 80-95: Phrygian
 * - 96-111: Locrian
 * - 112-127: Chromatic
 */
class Mode5_BasslineProgression : public Mode {
private:
  // Bass note range: C1 (36) to C4 (72)
  static constexpr uint8_t MIN_NOTE = 36;  // C1
  static constexpr uint8_t MAX_NOTE = 72;  // C4
  static constexpr uint8_t NOTE_RANGE = MAX_NOTE - MIN_NOTE;

  // Base velocity
  static constexpr uint8_t BASE_VELOCITY = 110;
  static constexpr uint8_t ACCENT_VELOCITY = 127;

  // Scale intervals (semitones from root)
  static constexpr uint8_t SCALE_MAJOR[7] = {0, 2, 4, 5, 7, 9, 11};
  static constexpr uint8_t SCALE_MINOR[7] = {0, 2, 3, 5, 7, 8, 10};
  static constexpr uint8_t SCALE_DORIAN[7] = {0, 2, 3, 5, 7, 9, 10};
  static constexpr uint8_t SCALE_MIXOLYDIAN[7] = {0, 2, 4, 5, 7, 9, 10};
  static constexpr uint8_t SCALE_BLUES[6] = {0, 3, 5, 6, 7, 10};
  static constexpr uint8_t SCALE_PHRYGIAN[7] = {0, 1, 3, 5, 7, 8, 10};
  static constexpr uint8_t SCALE_LOCRIAN[7] = {0, 1, 3, 5, 6, 8, 10};
  static constexpr uint8_t SCALE_CHROMATIC[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

  // Pattern timing (subdivisions within a step at 120 BPM)
  static constexpr unsigned long STEP_MS = 125;  // 16th note at 120 BPM

  /**
   * Get scale degree note
   */
  uint8_t getScaleNote(uint8_t root, const uint8_t* scale, uint8_t scaleLength, int8_t degree) const {
    // Handle negative degrees (below root)
    while (degree < 0) {
      degree += scaleLength;
      root -= 12;
    }

    // Calculate octave and position
    uint8_t octave = degree / scaleLength;
    uint8_t position = degree % scaleLength;

    int16_t note = root + (octave * 12) + scale[position];

    // Clamp to valid range
    if (note < 0) note = 0;
    if (note > 127) note = 127;

    return (uint8_t)note;
  }

public:
  Mode5_BasslineProgression(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (trackIndex >= 8) return;

    // Only process if switch is active
    if (!event.getSwitch()) return;

    // Read parameters
    uint8_t rootValue = event.getPot(0);       // Slider 0: Root note
    uint8_t scaleValue = event.getPot(1);      // Slider 1: Scale type
    uint8_t patternValue = event.getPot(2);    // Slider 2: Bass pattern
    uint8_t durationValue = event.getPot(3);   // Slider 3: Note duration

    // Map root to bass range (C1-C4)
    uint8_t rootNote = MIN_NOTE + ((rootValue * NOTE_RANGE) / 127);
    if (rootNote > MAX_NOTE) rootNote = MAX_NOTE;

    // Select scale
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
      scale = SCALE_MIXOLYDIAN;
      scaleLength = 7;
    } else if (scaleValue < 80) {
      scale = SCALE_BLUES;
      scaleLength = 6;
    } else if (scaleValue < 96) {
      scale = SCALE_PHRYGIAN;
      scaleLength = 7;
    } else if (scaleValue < 112) {
      scale = SCALE_LOCRIAN;
      scaleLength = 7;
    } else {
      scale = SCALE_CHROMATIC;
      scaleLength = 12;
    }

    // Map duration (50ms-1000ms)
    unsigned long noteDuration = 50 + ((unsigned long)durationValue * 950) / 127;

    // Generate bass pattern based on patternValue
    unsigned long delay = 0;

    if (patternValue < 16) {
      // Root only (whole note)
      uint8_t note = getScaleNote(rootNote, scale, scaleLength, 0);
      output.noteOn(midiChannel, note, ACCENT_VELOCITY, delay);
      output.noteOff(midiChannel, note, delay + noteDuration);

    } else if (patternValue < 32) {
      // Root + Fifth (alternating)
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t fifth = getScaleNote(rootNote, scale, scaleLength, 4);

      output.noteOn(midiChannel, root, ACCENT_VELOCITY, 0);
      output.noteOff(midiChannel, root, noteDuration);
      output.noteOn(midiChannel, fifth, BASE_VELOCITY, STEP_MS / 2);
      output.noteOff(midiChannel, fifth, STEP_MS / 2 + noteDuration);

    } else if (patternValue < 48) {
      // Root + Fifth + Octave (walking up)
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t fifth = getScaleNote(rootNote, scale, scaleLength, 4);
      uint8_t octave = getScaleNote(rootNote, scale, scaleLength, 7);

      output.noteOn(midiChannel, root, ACCENT_VELOCITY, 0);
      output.noteOff(midiChannel, root, noteDuration);
      output.noteOn(midiChannel, fifth, BASE_VELOCITY, STEP_MS / 3);
      output.noteOff(midiChannel, fifth, STEP_MS / 3 + noteDuration);
      output.noteOn(midiChannel, octave, BASE_VELOCITY, STEP_MS * 2 / 3);
      output.noteOff(midiChannel, octave, STEP_MS * 2 / 3 + noteDuration);

    } else if (patternValue < 64) {
      // Root + Third + Fifth (triad walk)
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t third = getScaleNote(rootNote, scale, scaleLength, 2);
      uint8_t fifth = getScaleNote(rootNote, scale, scaleLength, 4);

      output.noteOn(midiChannel, root, ACCENT_VELOCITY, 0);
      output.noteOff(midiChannel, root, noteDuration);
      output.noteOn(midiChannel, third, BASE_VELOCITY, STEP_MS / 3);
      output.noteOff(midiChannel, third, STEP_MS / 3 + noteDuration);
      output.noteOn(midiChannel, fifth, BASE_VELOCITY, STEP_MS * 2 / 3);
      output.noteOff(midiChannel, fifth, STEP_MS * 2 / 3 + noteDuration);

    } else if (patternValue < 80) {
      // Root + Third + Fifth + Seventh (jazz walk)
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t third = getScaleNote(rootNote, scale, scaleLength, 2);
      uint8_t fifth = getScaleNote(rootNote, scale, scaleLength, 4);
      uint8_t seventh = getScaleNote(rootNote, scale, scaleLength, 6);

      output.noteOn(midiChannel, root, ACCENT_VELOCITY, 0);
      output.noteOff(midiChannel, root, noteDuration);
      output.noteOn(midiChannel, third, BASE_VELOCITY, STEP_MS / 4);
      output.noteOff(midiChannel, third, STEP_MS / 4 + noteDuration);
      output.noteOn(midiChannel, fifth, BASE_VELOCITY, STEP_MS / 2);
      output.noteOff(midiChannel, fifth, STEP_MS / 2 + noteDuration);
      output.noteOn(midiChannel, seventh, BASE_VELOCITY, STEP_MS * 3 / 4);
      output.noteOff(midiChannel, seventh, STEP_MS * 3 / 4 + noteDuration);

    } else if (patternValue < 96) {
      // Octave bounce (root low, root high)
      uint8_t rootLow = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t rootHigh = getScaleNote(rootNote, scale, scaleLength, 7);

      output.noteOn(midiChannel, rootLow, ACCENT_VELOCITY, 0);
      output.noteOff(midiChannel, rootLow, noteDuration);
      output.noteOn(midiChannel, rootHigh, BASE_VELOCITY, STEP_MS / 2);
      output.noteOff(midiChannel, rootHigh, STEP_MS / 2 + noteDuration);

    } else if (patternValue < 112) {
      // Chromatic approach (half-step below to root)
      uint8_t approach = rootNote - 1;
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);

      output.noteOn(midiChannel, approach, BASE_VELOCITY - 20, 0);
      output.noteOff(midiChannel, approach, noteDuration / 2);
      output.noteOn(midiChannel, root, ACCENT_VELOCITY, STEP_MS / 4);
      output.noteOff(midiChannel, root, STEP_MS / 4 + noteDuration);

    } else {
      // Fifth pedal (fifth on beats, root on offbeats)
      uint8_t root = getScaleNote(rootNote, scale, scaleLength, 0);
      uint8_t fifth = getScaleNote(rootNote, scale, scaleLength, 4);

      output.noteOn(midiChannel, fifth, BASE_VELOCITY, 0);
      output.noteOff(midiChannel, fifth, noteDuration);
      output.noteOn(midiChannel, root, ACCENT_VELOCITY, STEP_MS / 2);
      output.noteOff(midiChannel, root, STEP_MS / 2 + noteDuration);
    }

    // Unused parameter
    (void)stepTime;
  }

  const char* getName() const override {
    return "BassLine";
  }
};

#endif // MODE5_BASSLINEPROGRESSION_H
