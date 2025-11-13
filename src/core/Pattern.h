#ifndef PATTERN_H
#define PATTERN_H

#include "Track.h"

/**
 * Pattern - A collection of 8 parallel Tracks
 *
 * Each pattern contains 8 tracks that play simultaneously.
 * All tracks in a pattern share the same tempo and loop length.
 *
 * Memory: 8 tracks × 64 bytes = 512 bytes per pattern
 */
class Pattern {
private:
  static constexpr uint8_t NUM_TRACKS = 8;
  Track tracks[NUM_TRACKS];

public:
  Pattern() {}

  // Track access
  inline Track& getTrack(uint8_t index) {
    return tracks[index & 0x07];  // Mask to 0-7
  }

  inline const Track& getTrack(uint8_t index) const {
    return tracks[index & 0x07];
  }

  // Direct access operator
  inline Track& operator[](uint8_t index) {
    return getTrack(index);
  }

  inline const Track& operator[](uint8_t index) const {
    return getTrack(index);
  }

  // Clear all tracks
  void clear() {
    for (uint8_t i = 0; i < NUM_TRACKS; i++) {
      tracks[i].clear();
    }
  }

  // Check if pattern has any active events
  bool hasActiveEvents() const {
    for (uint8_t i = 0; i < NUM_TRACKS; i++) {
      if (tracks[i].hasActiveEvents()) {
        return true;
      }
    }
    return false;
  }

  static constexpr uint8_t getNumTracks() { return NUM_TRACKS; }
};

#endif // PATTERN_H
