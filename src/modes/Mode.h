#ifndef MODE_H
#define MODE_H

#include "../core/Event.h"
#include "../hardware/InputState.h"
#include <stdint.h>

// Forward declarations
class MIDIScheduler;

/**
 * Mode - Base class for all GRUVBOK modes
 *
 * Each mode interprets Event data differently, creating unique musical behaviors.
 * Modes communicate with the sequencer through the MIDI API functions.
 *
 * Mode API (provided by MIDIScheduler):
 * - note(pitch, velocity, delta=0)  : Send note on
 * - off(pitch, delta=0)             : Send note off
 * - cc(controller, value, delta=0)  : Send CC message
 * - stopall(delta=0)                : All notes off
 *
 * Delta timing: All deltas are relative to current event time (milliseconds)
 */
class Mode {
protected:
  uint8_t midiChannel;     // MIDI channel for this mode (1-16)
  MIDIScheduler* scheduler; // Reference to MIDI scheduler

public:
  Mode(uint8_t channel) : midiChannel(channel), scheduler(nullptr) {}
  virtual ~Mode() {}

  // Initialize mode with MIDI scheduler
  void init(MIDIScheduler* sched) {
    scheduler = sched;
  }

  /**
   * PLAYBACK: Pure function - transform event data into MIDI
   *
   * This is the ONLY mode-specific logic.
   * Each mode interprets the raw event data (4 pots) differently.
   *
   * @param trackIndex Track number (0-7)
   * @param event The event to process (raw pot values)
   * @param stepTime Current step time in milliseconds (for delta calculations)
   */
  virtual void processEvent(uint8_t trackIndex, const Event& event, unsigned long stepTime) = 0;

  /**
   * Called when mode is activated
   */
  virtual void onActivate() {}

  /**
   * Called when mode is deactivated
   */
  virtual void onDeactivate() {}

  /**
   * Get mode name (for debugging/UI)
   */
  virtual const char* getName() const = 0;

  uint8_t getChannel() const { return midiChannel; }
};

#endif // MODE_H
