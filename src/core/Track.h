#ifndef TRACK_H
#define TRACK_H

#include "Event.h"

/**
 * Track - A sequence of 16 Events
 *
 * Each track contains 16 events, matching the 16 hardware buttons.
 * Tracks loop continuously during playback.
 *
 * Memory: 16 events × 4 bytes = 64 bytes per track
 */
class Track {
private:
  static constexpr uint8_t NUM_EVENTS = 16;
  Event events[NUM_EVENTS];

public:
  Track() {
    clear();
  }

  // Event access
  inline Event& getEvent(uint8_t index) {
    return events[index & 0x0F];  // Mask to 0-15
  }

  inline const Event& getEvent(uint8_t index) const {
    return events[index & 0x0F];
  }

  // Direct access operator
  inline Event& operator[](uint8_t index) {
    return getEvent(index);
  }

  inline const Event& operator[](uint8_t index) const {
    return getEvent(index);
  }

  // Clear all events
  void clear() {
    for (uint8_t i = 0; i < NUM_EVENTS; i++) {
      events[i].clear();
    }
  }

  // Check if track has any active events
  bool hasActiveEvents() const {
    for (uint8_t i = 0; i < NUM_EVENTS; i++) {
      if (events[i].getSwitch()) {
        return true;
      }
    }
    return false;
  }

  // Count active events
  uint8_t countActiveEvents() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < NUM_EVENTS; i++) {
      if (events[i].getSwitch()) {
        count++;
      }
    }
    return count;
  }

  static constexpr uint8_t getNumEvents() { return NUM_EVENTS; }
};

#endif // TRACK_H
