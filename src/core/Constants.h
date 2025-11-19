#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <stdint.h>

/**
 * Constants.h - Global constants for GRUVBOK
 *
 * Centralizes all magic numbers and configuration values
 * for better maintainability and extensibility.
 */

namespace GRUVBOK {

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

namespace Hardware {
  static constexpr uint8_t NUM_BUTTONS = 16;
  static constexpr uint8_t NUM_POTS = 4;
  static constexpr uint8_t NUM_SLIDERS = 4;

  // ADC Configuration
  static constexpr uint16_t ADC_MAX = 1023;           // 10-bit ADC
  static constexpr uint8_t MIDI_MAX = 127;            // 7-bit MIDI
  static constexpr uint8_t ADC_TO_MIDI_SHIFT = 3;     // Divide by 8 (1024/128)

  // Smoothing
  static constexpr float SMOOTHING_FACTOR = 0.3f;     // EMA alpha (0-1)

  // Debouncing
  static constexpr unsigned long DEBOUNCE_MS = 20;
}

// ============================================================================
// SONG STRUCTURE
// ============================================================================

namespace Song {
  static constexpr uint8_t NUM_MODES = 15;            // MIDI channels 1-15
  static constexpr uint8_t NUM_PATTERNS = 32;         // Patterns per mode
  static constexpr uint8_t NUM_TRACKS = 8;            // Tracks per pattern
  static constexpr uint8_t NUM_STEPS = 16;            // Steps per track

  // Bit packing
  static constexpr uint8_t BITS_PER_POT = 7;          // 0-127 range
  static constexpr uint8_t NUM_POTS = 4;              // Per event

  // Memory calculations
  static constexpr size_t EVENT_SIZE = 4;             // bytes
  static constexpr size_t TRACK_SIZE = EVENT_SIZE * NUM_STEPS;
  static constexpr size_t PATTERN_SIZE = TRACK_SIZE * NUM_TRACKS;
  static constexpr size_t SONG_SIZE = PATTERN_SIZE * NUM_PATTERNS * NUM_MODES;
}

// ============================================================================
// MIDI CONFIGURATION
// ============================================================================

namespace MIDI {
  static constexpr uint8_t MIN_CHANNEL = 1;
  static constexpr uint8_t MAX_CHANNEL = 16;
  static constexpr uint8_t MIN_NOTE = 0;
  static constexpr uint8_t MAX_NOTE = 127;
  static constexpr uint8_t MIN_VELOCITY = 0;
  static constexpr uint8_t MAX_VELOCITY = 127;
  static constexpr uint8_t MIN_CC = 0;
  static constexpr uint8_t MAX_CC = 127;

  // MIDI Clock
  static constexpr uint8_t PULSES_PER_QUARTER = 24;   // PPQN

  // Scheduler
  static constexpr uint8_t MAX_SCHEDULED_EVENTS = 64;
}

// ============================================================================
// TIMING
// ============================================================================

namespace Timing {
  // BPM Range
  static constexpr float MIN_BPM = 20.0f;
  static constexpr float MAX_BPM = 800.0f;
  static constexpr float DEFAULT_BPM = 120.0f;

  // Step timing
  static constexpr uint8_t STEPS_PER_BEAT = 4;        // 16th notes

  // Calculations
  inline constexpr unsigned long calculateStepInterval(float bpm) {
    return static_cast<unsigned long>((60000.0f / bpm) / STEPS_PER_BEAT);
  }

  inline constexpr unsigned long calculateClockInterval(float bpm) {
    return static_cast<unsigned long>((60000.0f / bpm) / MIDI::PULSES_PER_QUARTER);
  }
}

// ============================================================================
// MODE DEFAULTS
// ============================================================================

namespace Mode {
  // Mode channels (1-indexed for MIDI)
  static constexpr uint8_t PATTERN_SEQUENCER_CHANNEL = 1;   // Mode 0
  static constexpr uint8_t DRUM_MACHINE_CHANNEL = 2;        // Mode 1

  // Mode 1: Drum Machine
  namespace DrumMachine {
    static constexpr uint8_t NUM_DRUM_TRACKS = 8;

    // GM Drum Note Mapping
    static constexpr uint8_t KICK_NOTE = 36;           // C1
    static constexpr uint8_t SNARE_NOTE = 38;          // D1
    static constexpr uint8_t CLOSED_HAT_NOTE = 42;     // F#1
    static constexpr uint8_t OPEN_HAT_NOTE = 46;       // A#1
    static constexpr uint8_t LOW_TOM_NOTE = 43;        // G1
    static constexpr uint8_t MID_TOM_NOTE = 47;        // B1
    static constexpr uint8_t CRASH_NOTE = 49;          // C#2
    static constexpr uint8_t RIDE_NOTE = 51;           // D#2

    // Parameter ranges (ms)
    static constexpr unsigned long MIN_NOTE_LENGTH = 10;
    static constexpr unsigned long MAX_NOTE_LENGTH = 2000;
    static constexpr unsigned long MIN_FLAM_DELAY = 5;
    static constexpr unsigned long MAX_FLAM_DELAY = 50;

    // Default values
    static constexpr uint8_t DEFAULT_VELOCITY = 100;
    static constexpr uint8_t FLAM_VELOCITY_FACTOR = 60;  // Percent of main velocity
  }
}

// ============================================================================
// UI/DEBUG
// ============================================================================

namespace Debug {
  // MIDI CC channels for debug output
  static constexpr uint8_t DEBUG_CHANNEL = 16;

  // Navigation CCs (on channel 16)
  static constexpr uint8_t CC_MODE = 1;
  static constexpr uint8_t CC_PATTERN = 2;
  static constexpr uint8_t CC_TRACK = 3;

  // Slider CCs (on drum machine channel)
  static constexpr uint8_t CC_SLIDER_BASE = 20;     // CCs 20-23

  // Update intervals
  static constexpr unsigned long SLIDER_DEBUG_INTERVAL_MS = 50;
}

namespace LED {
  static constexpr uint8_t DOWNBEAT_BRIGHTNESS = 255;  // Bright on step 0 (downbeat)
  static constexpr uint8_t BEAT_BRIGHTNESS = 50;       // Medium on steps 4, 8, 12 (quarter notes)
  static constexpr uint8_t OFFBEAT_BRIGHTNESS = 5;     // Soft on all other steps
}

// ============================================================================
// VALIDATION HELPERS
// ============================================================================

inline bool isValidMIDIChannel(uint8_t channel) {
  return channel >= MIDI::MIN_CHANNEL && channel <= MIDI::MAX_CHANNEL;
}

inline bool isValidMIDINote(uint8_t note) {
  return note <= MIDI::MAX_NOTE;
}

inline bool isValidMIDIValue(uint8_t value) {
  return value <= MIDI::MAX_VELOCITY;
}

inline uint8_t clampMIDIValue(uint8_t value) {
  return (value > MIDI::MAX_VELOCITY) ? MIDI::MAX_VELOCITY : value;
}

inline float clampBPM(float bpm) {
  if (bpm < Timing::MIN_BPM) return Timing::MIN_BPM;
  if (bpm > Timing::MAX_BPM) return Timing::MAX_BPM;
  return bpm;
}

}  // namespace GRUVBOK

#endif  // CONSTANTS_H
