#ifndef SONG_H
#define SONG_H

#include <stddef.h>  // For size_t
#include "Pattern.h"

/**
 * Song - The complete GRUVBOK data structure
 *
 * A Song contains 15 Modes (MIDI channels 1-15).
 * Each Mode contains 32 Patterns.
 * Mode 0 is special: it controls which pattern plays on all other modes.
 *
 * Memory: 15 modes × 32 patterns × 512 bytes = 245,760 bytes (~240 KB)
 *
 * Structure:
 * Song
 *   └─ Mode[15]
 *       └─ Pattern[32]
 *           └─ Track[8]
 *               └─ Event[16]
 */
class Song {
private:
  static constexpr uint8_t NUM_MODES = 15;
  static constexpr uint8_t NUM_PATTERNS = 32;

  Pattern patterns[NUM_MODES][NUM_PATTERNS];

public:
  Song() {}

  // Pattern access by mode and pattern index
  inline Pattern& getPattern(uint8_t mode, uint8_t pattern) {
    return patterns[mode & 0x0F][pattern & 0x1F];  // Mask mode to 0-14, pattern to 0-31
  }

  inline const Pattern& getPattern(uint8_t mode, uint8_t pattern) const {
    return patterns[mode & 0x0F][pattern & 0x1F];
  }

  // Clear entire song
  void clear() {
    for (uint8_t m = 0; m < NUM_MODES; m++) {
      for (uint8_t p = 0; p < NUM_PATTERNS; p++) {
        patterns[m][p].clear();
      }
    }
  }

  // Get memory size
  static constexpr size_t getMemorySize() {
    return NUM_MODES * NUM_PATTERNS * sizeof(Pattern);
  }

  static constexpr uint8_t getNumModes() { return NUM_MODES; }
  static constexpr uint8_t getNumPatterns() { return NUM_PATTERNS; }
};

#endif // SONG_H
