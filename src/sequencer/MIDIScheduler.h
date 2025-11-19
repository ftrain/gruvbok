#ifndef MIDISCHEDULER_H
#define MIDISCHEDULER_H

#include <stdint.h>
#include "../core/MIDIEvent.h"

/**
 * MIDIScheduler - Manages scheduled MIDI events with delta timing
 *
 * NEW: Supports bulk scheduling from MIDIEventBuffer for pure functional modes.
 *
 * Individual methods (for compatibility):
 * - note(pitch, velocity, delta)
 * - off(pitch, delta)
 * - cc(controller, value, delta)
 * - stopall(delta)
 *
 * Bulk method (preferred):
 * - scheduleAll(MIDIEventBuffer)
 *
 * Events are scheduled relative to the current time + delta offset.
 */
class MIDIScheduler {
private:
  struct ScheduledEvent {
    enum Type { NOTE_ON, NOTE_OFF, CC, STOP_ALL };

    Type type;
    uint8_t channel;
    uint8_t data1;  // pitch/controller
    uint8_t data2;  // velocity/value
    unsigned long executeTime;
    bool active;

    ScheduledEvent() : active(false) {}
  };

  static constexpr uint8_t MAX_SCHEDULED_EVENTS = 64;
  ScheduledEvent events[MAX_SCHEDULED_EVENTS];

public:
  MIDIScheduler() {
    for (uint8_t i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      events[i].active = false;
    }
  }

  /**
   * Schedule a note on event
   * @param channel MIDI channel (1-16)
   * @param pitch MIDI note (0-127)
   * @param velocity Note velocity (0-127)
   * @param delta Delay in milliseconds from current time
   */
  void note(uint8_t channel, uint8_t pitch, uint8_t velocity, unsigned long delta = 0);

  /**
   * Schedule a note off event
   * @param channel MIDI channel (1-16)
   * @param pitch MIDI note (0-127)
   * @param delta Delay in milliseconds from current time
   */
  void off(uint8_t channel, uint8_t pitch, unsigned long delta = 0);

  /**
   * Schedule a CC (control change) event
   * @param channel MIDI channel (1-16)
   * @param controller CC number (0-127)
   * @param value CC value (0-127)
   * @param delta Delay in milliseconds from current time
   */
  void cc(uint8_t channel, uint8_t controller, uint8_t value, unsigned long delta = 0);

  /**
   * Schedule all notes off
   * @param channel MIDI channel (1-16)
   * @param delta Delay in milliseconds from current time
   */
  void stopall(uint8_t channel, unsigned long delta = 0);

  /**
   * Schedule all events from a buffer (PREFERRED METHOD)
   * Bulk scheduling for pure functional modes
   * @param buffer MIDIEventBuffer containing events to schedule
   * @return Number of events successfully scheduled
   */
  uint8_t scheduleAll(const MIDIEventBuffer& buffer);

  /**
   * Process scheduled events - call this frequently in main loop
   */
  void update();

  /**
   * Clear all scheduled events
   */
  void clear();

private:
  // Find free slot in event buffer
  int8_t findFreeSlot();

  // Schedule generic event
  void scheduleEvent(ScheduledEvent::Type type, uint8_t channel,
                    uint8_t data1, uint8_t data2, unsigned long delta);
};

#endif // MIDISCHEDULER_H
