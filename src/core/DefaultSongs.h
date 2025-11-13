#ifndef DEFAULTSONGS_H
#define DEFAULTSONGS_H

#include "Song.h"

/**
 * DefaultSongs - Pre-programmed patterns and songs
 *
 * Provides factory patterns that sound good out of the box.
 * Demonstrates GRUVBOK's capabilities immediately on power-up.
 */
class DefaultSongs {
public:
  /**
   * Load a basic demo song
   *
   * Mode1 (Drum Machine):
   * - Track 0 (Kick): Four-on-the-floor
   * - Track 1 (Snare): Backbeat (steps 4, 12)
   * - Track 2 (Closed Hat): 8th notes
   * - Track 3 (Open Hat): Accents
   */
  static void loadDemoSong(Song& song);

  /**
   * Load a techno pattern
   *
   * Mode1: Classic 909-style techno beat
   */
  static void loadTechnoPattern(Song& song);

  /**
   * Load a breakbeat pattern
   *
   * Mode1: Funky breakbeat
   */
  static void loadBreakbeat(Song& song);

  /**
   * Clear song and load silence
   */
  static void loadEmpty(Song& song);

private:
  // Helper: Set event with switch + pots
  static void setEvent(Pattern& pattern, uint8_t track, uint8_t step,
                      bool sw, uint8_t pot0 = 100, uint8_t pot1 = 64,
                      uint8_t pot2 = 0, uint8_t pot3 = 0);
};

#endif // DEFAULTSONGS_H
