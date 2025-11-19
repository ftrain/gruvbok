#ifndef MODE_H
#define MODE_H

#include "../core/Event.h"
#include "../core/MIDIEvent.h"
#include "../hardware/InputState.h"
#include <stdint.h>

/**
 * Mode - Base class for all GRUVBOK modes
 *
 * PURE FUNCTIONAL DESIGN:
 * Modes are truly pure functions that transform Event data into MIDIEvent data.
 * They have NO SIDE EFFECTS - no scheduling, no state mutation, no I/O.
 *
 * Input:  Event (switch + 4 pots)
 * Output: MIDIEventBuffer (list of MIDI messages)
 *
 * The sequencer collects events from all modes, then schedules them in bulk.
 * This enables:
 * - True functional purity (no side effects)
 * - Easy testing (no mocking needed)
 * - Parallel processing (no shared state)
 * - Event filtering/transformation
 * - Priority handling by scheduler
 */
class Mode {
protected:
  uint8_t midiChannel;     // MIDI channel for this mode (1-16)

public:
  Mode(uint8_t channel) : midiChannel(channel) {}
  virtual ~Mode() {}

  /**
   * PLAYBACK: Pure function - transform event data into MIDI events
   *
   * This is the ONLY mode-specific logic.
   * Each mode interprets the raw event data (4 pots) differently
   * and generates MIDI events into the output buffer.
   *
   * MUST BE PURE:
   * - No side effects (no scheduling, no I/O)
   * - No state mutation (const event, read-only members)
   * - Deterministic (same input = same output)
   * - No exceptions/errors (embedded systems)
   *
   * @param trackIndex Track number (0-7)
   * @param event The event to process (raw pot values)
   * @param stepTime Current step time in milliseconds (for delta calculations)
   * @param output Buffer to write MIDI events into
   */
  virtual void processEvent(uint8_t trackIndex, const Event& event,
                           unsigned long stepTime, MIDIEventBuffer& output) const = 0;

  /**
   * Called when mode is activated (optional lifecycle hook)
   */
  virtual void onActivate() {}

  /**
   * Called when mode is deactivated (optional lifecycle hook)
   */
  virtual void onDeactivate() {}

  /**
   * Get mode name (for debugging/UI)
   */
  virtual const char* getName() const = 0;

  /**
   * Get MIDI channel for this mode
   */
  uint8_t getChannel() const { return midiChannel; }
};

#endif // MODE_H
