#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "../core/Song.h"
#include "../hardware/Hardware.h"
#include "MIDIScheduler.h"
#include "../modes/Mode.h"

/**
 * Sequencer - The heart of GRUVBOK
 *
 * Manages:
 * - Song playback (always playing, always looping)
 * - Current position tracking (mode, pattern, track, step)
 * - Timing and tempo
 * - Mode coordination
 * - User input handling (button/pot → Event recording)
 * - MIDI clock output
 *
 * Dataflow:
 * 1. User presses button → Event is recorded at current step
 * 2. Sequencer advances through steps based on tempo
 * 3. For each step, all active modes process their events
 * 4. Modes schedule MIDI via MIDIScheduler
 * 5. MIDIScheduler executes MIDI at scheduled times
 */
class Sequencer {
private:
  Song* song;                    // The complete song data
  Hardware* hardware;            // Hardware I/O
  MIDIScheduler* scheduler;      // MIDI event scheduler
  Mode* modes[15];               // Array of mode instances

  // Playback state
  uint8_t currentPatterns[15];   // Current pattern per mode (Mode0 can change these)
  uint8_t currentStep;           // Current step (0-15)
  uint8_t currentTrack;          // Currently selected track for editing (0-7)
  uint8_t currentMode;           // Currently selected mode for editing (0-14)

  // Timing
  float bpm;                     // Current tempo
  unsigned long stepInterval;    // Time between steps (ms)
  unsigned long lastStepTime;    // Last step advance time
  unsigned long clockInterval;   // MIDI clock interval (ms)
  unsigned long lastClockTime;   // Last MIDI clock time

  // MIDI Clock
  bool sendClock;                // Enable/disable MIDI clock output

  // State
  bool isPlaying;                // Playback state

public:
  Sequencer(Song* s, Hardware* hw, MIDIScheduler* sched);
  ~Sequencer();

  /**
   * Initialize sequencer and all modes
   */
  void init();

  /**
   * Main update loop - call this frequently
   * Handles timing, step advancement, event processing
   */
  void update();

  /**
   * Start playback
   */
  void start();

  /**
   * Stop playback
   */
  void stop();

  /**
   * Set tempo
   * @param newBPM Tempo in beats per minute (60-240)
   */
  void setBPM(float newBPM);

  /**
   * Get current tempo
   */
  float getBPM() const { return bpm; }

  /**
   * Set current track for editing
   */
  void setCurrentTrack(uint8_t track) {
    currentTrack = track & 0x07;
  }

  /**
   * Set current mode for editing
   */
  void setCurrentMode(uint8_t mode) {
    currentMode = mode & 0x0F;
  }

  /**
   * Get current step
   */
  uint8_t getCurrentStep() const { return currentStep; }

  /**
   * Get current track
   */
  uint8_t getCurrentTrack() const { return currentTrack; }

  /**
   * Get current mode
   */
  uint8_t getCurrentMode() const { return currentMode; }

  /**
   * Enable/disable MIDI clock
   */
  void setClockEnabled(bool enabled) { sendClock = enabled; }

private:
  /**
   * Advance to next step
   */
  void advanceStep();

  /**
   * Process current step across all modes
   */
  void processStep();

  /**
   * Send MIDI clock pulse
   */
  void sendClockPulse();

  /**
   * Calculate timing intervals from BPM
   */
  void calculateIntervals();

  /**
   * Handle user input (buttons/pots)
   */
  void handleInput();

  /**
   * Record event at current position
   */
  void recordEvent(uint8_t buttonIndex, bool state);
};

#endif // SEQUENCER_H
