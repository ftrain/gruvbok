#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <stdint.h>
#include "Constants.h"

/**
 * MIDIEvent - A scheduled MIDI message
 *
 * Represents a single MIDI message to be sent at a specific delta time.
 * Pure data structure with no behavior - perfect for functional programming.
 */
struct MIDIEvent {
  enum Type : uint8_t {
    NOTE_ON = 0,
    NOTE_OFF = 1,
    CC = 2,
    STOP_ALL = 3
  };

  Type type;
  uint8_t channel;       // MIDI channel (1-16)
  uint8_t data1;         // Note/controller number (0-127)
  uint8_t data2;         // Velocity/value (0-127)
  unsigned long delta;   // Delay from current time (ms)

  // Default constructor
  MIDIEvent() : type(NOTE_ON), channel(1), data1(0), data2(0), delta(0) {}

  // Parameterized constructor
  MIDIEvent(Type t, uint8_t ch, uint8_t d1, uint8_t d2, unsigned long d)
    : type(t), channel(ch), data1(d1), data2(d2), delta(d) {}

  // Factory methods for clarity
  static MIDIEvent noteOn(uint8_t channel, uint8_t note, uint8_t velocity, unsigned long delta = 0) {
    return MIDIEvent(NOTE_ON, channel, note, velocity, delta);
  }

  static MIDIEvent noteOff(uint8_t channel, uint8_t note, unsigned long delta = 0) {
    return MIDIEvent(NOTE_OFF, channel, note, 0, delta);
  }

  static MIDIEvent cc(uint8_t channel, uint8_t controller, uint8_t value, unsigned long delta = 0) {
    return MIDIEvent(CC, channel, controller, value, delta);
  }

  static MIDIEvent stopAll(uint8_t channel, unsigned long delta = 0) {
    return MIDIEvent(STOP_ALL, channel, 0, 0, delta);
  }
};

/**
 * MIDIEventBuffer - Fixed-size buffer for MIDI events
 *
 * Embedded-friendly event collection with no dynamic allocation.
 * Modes return events into this buffer, then sequencer schedules them all.
 *
 * Design principles:
 * - Fixed size (stack-allocated)
 * - No dynamic memory
 * - Bounds checking
 * - Simple iterator interface
 */
class MIDIEventBuffer {
private:
  static constexpr uint8_t MAX_EVENTS = 32;  // Per step, all modes combined
  MIDIEvent events[MAX_EVENTS];
  uint8_t count;

public:
  MIDIEventBuffer() : count(0) {}

  /**
   * Add an event to the buffer
   * @return true if added, false if buffer full
   */
  bool add(const MIDIEvent& event) {
    if (count >= MAX_EVENTS) return false;
    events[count++] = event;
    return true;
  }

  /**
   * Add an event using parameters
   */
  bool add(MIDIEvent::Type type, uint8_t channel, uint8_t data1, uint8_t data2, unsigned long delta) {
    return add(MIDIEvent(type, channel, data1, data2, delta));
  }

  /**
   * Add a note on event
   */
  bool noteOn(uint8_t channel, uint8_t note, uint8_t velocity, unsigned long delta = 0) {
    return add(MIDIEvent::noteOn(channel, note, velocity, delta));
  }

  /**
   * Add a note off event
   */
  bool noteOff(uint8_t channel, uint8_t note, unsigned long delta = 0) {
    return add(MIDIEvent::noteOff(channel, note, delta));
  }

  /**
   * Add a CC event
   */
  bool cc(uint8_t channel, uint8_t controller, uint8_t value, unsigned long delta = 0) {
    return add(MIDIEvent::cc(channel, controller, value, delta));
  }

  /**
   * Add a stop all event
   */
  bool stopAll(uint8_t channel, unsigned long delta = 0) {
    return add(MIDIEvent::stopAll(channel, delta));
  }

  /**
   * Clear all events
   */
  void clear() {
    count = 0;
  }

  /**
   * Get number of events in buffer
   */
  uint8_t size() const {
    return count;
  }

  /**
   * Check if buffer is empty
   */
  bool isEmpty() const {
    return count == 0;
  }

  /**
   * Check if buffer is full
   */
  bool isFull() const {
    return count >= MAX_EVENTS;
  }

  /**
   * Get event by index (const)
   */
  const MIDIEvent& operator[](uint8_t index) const {
    return events[index];
  }

  /**
   * Get event by index (mutable)
   */
  MIDIEvent& operator[](uint8_t index) {
    return events[index];
  }

  /**
   * Iterator support - call function for each event
   */
  template<typename Func>
  void forEach(Func func) const {
    for (uint8_t i = 0; i < count; i++) {
      func(events[i]);
    }
  }

  /**
   * Get remaining capacity
   */
  uint8_t remaining() const {
    return MAX_EVENTS - count;
  }

  static constexpr uint8_t getMaxEvents() { return MAX_EVENTS; }
};

#endif  // MIDIEVENT_H
