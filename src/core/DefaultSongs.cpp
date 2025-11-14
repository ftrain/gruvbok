#include "DefaultSongs.h"

void DefaultSongs::setEvent(Pattern& pattern, uint8_t track, uint8_t step,
                           bool sw, uint8_t pot0, uint8_t pot1,
                           uint8_t pot2, uint8_t pot3) {
  Event& e = pattern[track][step];
  e.setSwitch(sw);
  e.setPot(0, pot0);  // Velocity
  e.setPot(1, pot1);  // Pan
  e.setPot(2, pot2);  // Tone/Filter
  e.setPot(3, pot3);  // Reverb
}

void DefaultSongs::loadDemoSong(Song& song) {
  // Clear everything first
  song.clear();

  // ============================================================================
  // Mode 0: Pattern Sequencer
  // ============================================================================
  // Initialize Mode 0 Pattern 0 Track 0 with default sequence:
  // Slot 0: Pattern 0 (loops pattern 0 forever)
  Pattern& mode0 = song.getPattern(0, 0);
  // Pot 0 = 0 maps to pattern 0 (0*32/128 = 0)
  setEvent(mode0, 0, 0, true, 0, 0, 0, 0);  // Sequence slot 0 -> pattern 0

  // ============================================================================
  // Mode 1: Drum Machine (Pattern 0)
  // ============================================================================
  Pattern& drums = song.getPattern(1, 0);

  // Track 0: Kick Drum on step 0 (button 1)
  setEvent(drums, 0, 0,  true, 127, 64, 0, 0);

  // Track 1: Snare Drum on step 8 (button 9)
  setEvent(drums, 1, 8,  true, 127, 64, 0, 0);

  // ============================================================================
  // Mode 2: Acid Bass (Pattern 0)
  // ============================================================================
  // Pattern 0 starts completely empty - blank canvas for acid programming
  // (No need to set anything - song.clear() already set all switches to off)
}

void DefaultSongs::loadTechnoPattern(Song& song) {
  song.clear();

  // Get Mode1, Pattern 0
  Pattern& drums = song.getPattern(1, 0);

  // Track 0: Kick (Four-on-the-floor, constant)
  for (uint8_t i = 0; i < 16; i += 4) {
    setEvent(drums, 0, i, true, 127, 64, 0, 0);
  }

  // Track 1: Snare (Clap on 2 and 4)
  setEvent(drums, 1, 4,  true, 100, 64, 0, 40);
  setEvent(drums, 1, 12, true, 100, 64, 0, 40);

  // Track 2: Closed Hi-Hat (16th notes)
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t vel = (i % 4 == 0) ? 110 : 70;  // Strong accent every 4
    setEvent(drums, 2, i, true, vel, 64, 30, 0);  // With filter
  }

  // Track 3: Open Hi-Hat (sparse, for space)
  setEvent(drums, 3, 2,  true, 80, 64, 0, 0);
  setEvent(drums, 3, 10, true, 75, 64, 0, 0);

  // Track 6: Crash (big moments)
  setEvent(drums, 6, 0, true, 120, 64, 0, 50);   // Start
  setEvent(drums, 6, 8, true, 100, 64, 0, 40);   // Halfway
}

void DefaultSongs::loadBreakbeat(Song& song) {
  song.clear();

  // Get Mode1, Pattern 0
  Pattern& drums = song.getPattern(1, 0);

  // Track 0: Kick (Funky placement)
  setEvent(drums, 0, 0,  true, 127, 64, 0, 0);
  setEvent(drums, 0, 3,  true, 100, 64, 0, 0);   // Swung
  setEvent(drums, 0, 8,  true, 120, 64, 0, 0);
  setEvent(drums, 0, 13, true, 90,  64, 0, 0);   // Ghost note

  // Track 1: Snare (Classic break pattern)
  setEvent(drums, 1, 4,  true, 120, 64, 0, 20);
  setEvent(drums, 1, 12, true, 127, 64, 0, 20);
  setEvent(drums, 1, 14, true, 100, 64, 0, 15);  // Flam

  // Track 2: Closed Hi-Hat (Funky 16ths)
  uint8_t hihatPattern[] = {0, 1, 2, 3, 4, 6, 8, 9, 10, 11, 12, 14, 15};
  for (uint8_t i = 0; i < 13; i++) {
    uint8_t step = hihatPattern[i];
    uint8_t vel = (step % 4 == 0) ? 100 : 75;
    setEvent(drums, 2, step, true, vel, 64, 0, 0);
  }

  // Track 3: Open Hi-Hat (Sparse accents)
  setEvent(drums, 3, 2,  true, 90, 64, 0, 10);
  setEvent(drums, 3, 10, true, 85, 64, 0, 10);

  // Track 4: Low Tom (Fill)
  setEvent(drums, 4, 7,  true, 100, 64, 0, 0);

  // Track 5: Mid Tom (Fill)
  setEvent(drums, 5, 15, true, 110, 64, 0, 0);
}

void DefaultSongs::loadEmpty(Song& song) {
  song.clear();
}
