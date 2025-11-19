#ifndef MODE3_EUCLIDEANFADE_H
#define MODE3_EUCLIDEANFADE_H

#include "Mode.h"

/**
 * Mode3 - Euclidean Fade (Generative Echo Sequencer)
 *
 * A creative mode that generates geometric echo patterns from single note triggers.
 * Notes echo over time with increasing spacing and pitch modulation, creating
 * evolving textures and harmonic layers.
 *
 * Event interpretation:
 * - Switch: Trigger note with echoes
 * - Slider 0: Pitch (0-127, maps to MIDI notes C1-C7, 24-96)
 * - Slider 1: Initial delay spacing (0-127, maps to 1-16 steps between echoes)
 * - Slider 2: Number of echoes/rate (0-127, maps to 1-8 echoes)
 * - Slider 3: Pitch modulation per echo (0-127, -12 to +12 semitones, centered at 64)
 *
 * Behavior:
 * - Base note plays immediately
 * - Echoes are spaced geometrically in steps: delay, delay*2, delay*4, delay*8, etc.
 * - Each echo's pitch shifts by the modulation amount
 * - Echoes fade in velocity (each echo is 80% of previous)
 * - All notes layer and accumulate for rich textures
 *
 * Example (at 120 BPM):
 * - Pitch: 60 (C4)
 * - Delay: 64 (4 steps)
 * - Rate: 4 echoes
 * - Pitch mod: 80 (+3 semitones per echo)
 * Result: C4 now, E4 at +4 steps, G#4 at +8 steps, C5 at +16 steps
 *        (creates a major chord progression over time)
 */
class Mode3_EuclideanFade : public Mode {
private:
  // Note range: C1 (24) to C7 (96)
  static constexpr uint8_t MIN_NOTE = 24;
  static constexpr uint8_t MAX_NOTE = 96;
  static constexpr uint8_t NOTE_RANGE = MAX_NOTE - MIN_NOTE;

  // Echo parameters
  static constexpr uint8_t MAX_ECHOES = 8;
  static constexpr uint8_t MIN_DELAY_STEPS = 1;
  static constexpr uint8_t MAX_DELAY_STEPS = 16;

  // Step timing (at 120 BPM default, one 16th note step = 125ms)
  // This provides a reference for converting steps to milliseconds
  static constexpr unsigned long MS_PER_STEP = 125;

  // Velocity fade per echo
  static constexpr float VELOCITY_FADE = 0.80f;  // Each echo is 80% of previous
  static constexpr uint8_t BASE_VELOCITY = 100;

public:
  Mode3_EuclideanFade(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (trackIndex >= 8) return;

    // Only process if switch is active
    if (!event.getSwitch()) return;

    // Read parameters from stored event sliders
    uint8_t pitchValue = event.getPot(0);      // Slider 0: Base pitch (0-127)
    uint8_t delayValue = event.getPot(1);      // Slider 1: Delay spacing (0-127)
    uint8_t rateValue = event.getPot(2);       // Slider 2: Number of echoes (0-127)
    uint8_t pitchModValue = event.getPot(3);   // Slider 3: Pitch mod per echo (0-127)

    // Map pitch to note range (C1-C7, MIDI 24-96)
    uint8_t baseNote = MIN_NOTE + ((pitchValue * NOTE_RANGE) / 127);
    if (baseNote > MAX_NOTE) baseNote = MAX_NOTE;

    // Map delay value to steps (1-16)
    uint8_t delaySteps = MIN_DELAY_STEPS +
                        (delayValue * (MAX_DELAY_STEPS - MIN_DELAY_STEPS)) / 127;
    if (delaySteps < 1) delaySteps = 1;

    // Convert steps to milliseconds
    unsigned long baseDelayMs = delaySteps * MS_PER_STEP;

    // Map rate to number of echoes (1-8)
    uint8_t numEchoes = 1 + (rateValue * (MAX_ECHOES - 1)) / 127;

    // Map pitch modulation to semitones (-12 to +12, centered at 64)
    int8_t pitchModSemitones = ((int16_t)pitchModValue - 64) * 12 / 64;

    // Generate base note and echoes with geometric spacing
    uint8_t velocity = BASE_VELOCITY;
    unsigned long echoDelay = 0;
    unsigned long geometricMultiplier = 1;

    for (uint8_t i = 0; i < numEchoes; i++) {
      // Calculate pitch for this echo
      int16_t echoNote = baseNote + (pitchModSemitones * i);

      // Clamp to valid MIDI range
      if (echoNote < 0) echoNote = 0;
      if (echoNote > 127) echoNote = 127;

      // Calculate velocity with fade
      uint8_t echoVelocity = velocity;
      if (echoVelocity < 10) echoVelocity = 10;  // Minimum audible velocity

      // Schedule note with geometric delay spacing (in steps)
      // Note length is half the delay spacing, but at least 50ms
      unsigned long noteLength = baseDelayMs / 2;
      if (noteLength < 50) noteLength = 50;

      output.noteOn(midiChannel, (uint8_t)echoNote, echoVelocity, echoDelay);
      output.noteOff(midiChannel, (uint8_t)echoNote, echoDelay + noteLength);

      // Geometric progression in steps: delay, delay*2, delay*4, delay*8, etc.
      echoDelay += baseDelayMs * geometricMultiplier;
      geometricMultiplier *= 2;

      // Fade velocity for next echo
      velocity = (uint8_t)(velocity * VELOCITY_FADE);
    }

    // Unused parameter
    (void)stepTime;
  }

  const char* getName() const override {
    return "EuclFade";
  }
};

#endif // MODE3_EUCLIDEANFADE_H
