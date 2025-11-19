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
  // Load the full song across patterns 1-12
  // Pattern 0 stays empty for testing
  loadFullSong(song);
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

void DefaultSongs::loadFullSong(Song& song) {
  song.clear();

  // ============================================================================
  // PATTERN 1: INTRO - Minimal kick + bass
  // ============================================================================
  Pattern& p1_drums = song.getPattern(1, 1);
  setEvent(p1_drums, 0, 0,  true, 120, 64, 100, 64);  // Kick on 1
  setEvent(p1_drums, 0, 8,  true, 120, 64, 100, 64);  // Kick on 9

  Pattern& p1_bass = song.getPattern(5, 1);
  setEvent(p1_bass, 0, 0,  true, 48, 0, 16, 100);     // C root only

  // ============================================================================
  // PATTERN 2: INTRO BUILD - Add hats
  // ============================================================================
  Pattern& p2_drums = song.getPattern(1, 2);
  setEvent(p2_drums, 0, 0,  true, 127, 64, 100, 64);  // Kick
  setEvent(p2_drums, 0, 8,  true, 127, 64, 100, 64);  // Kick
  setEvent(p2_drums, 2, 4,  true, 90, 64, 20, 64);    // Hi-hat
  setEvent(p2_drums, 2, 12, true, 90, 64, 20, 64);    // Hi-hat

  Pattern& p2_bass = song.getPattern(5, 2);
  setEvent(p2_bass, 0, 0,  true, 48, 0, 20, 100);     // C root + fifth
  setEvent(p2_bass, 0, 8,  true, 48, 0, 20, 100);     // C root + fifth

  // ============================================================================
  // PATTERN 3: BUILD - Add snare + more hats
  // ============================================================================
  Pattern& p3_drums = song.getPattern(1, 3);
  setEvent(p3_drums, 0, 0,  true, 127, 64, 80, 64);   // Kick
  setEvent(p3_drums, 0, 8,  true, 127, 64, 80, 64);   // Kick
  setEvent(p3_drums, 1, 4,  true, 120, 64, 100, 64);  // Snare
  setEvent(p3_drums, 1, 12, true, 120, 64, 100, 64);  // Snare
  for (uint8_t i = 0; i < 16; i += 2) {
    setEvent(p3_drums, 2, i, true, i % 4 == 0 ? 100 : 75, 64, 20, 64);  // 8th note hats
  }

  Pattern& p3_bass = song.getPattern(5, 3);
  setEvent(p3_bass, 0, 0,  true, 48, 0, 48, 90);      // C walking bass
  setEvent(p3_bass, 0, 4,  true, 55, 0, 48, 90);      // G walking bass

  // ============================================================================
  // PATTERN 4: MAIN GROOVE - Add arp
  // ============================================================================
  Pattern& p4_drums = song.getPattern(1, 4);
  // Four-on-the-floor kick
  for (uint8_t i = 0; i < 16; i += 4) {
    setEvent(p4_drums, 0, i, true, 127, 64, 50, 64);
  }
  // Backbeat snare
  setEvent(p4_drums, 1, 4,  true, 127, 64, 100, 64);
  setEvent(p4_drums, 1, 12, true, 127, 64, 100, 64);
  // 16th note hats
  for (uint8_t i = 0; i < 16; i++) {
    setEvent(p4_drums, 2, i, true, i % 4 == 0 ? 110 : 70, 64, 20, 64);
  }

  Pattern& p4_arp = song.getPattern(4, 4);
  setEvent(p4_arp, 0, 0,  true, 85, 20, 50, 64);      // C major arp
  setEvent(p4_arp, 0, 4,  true, 85, 20, 50, 64);      // C major arp

  Pattern& p4_bass = song.getPattern(5, 4);
  setEvent(p4_bass, 0, 0,  true, 48, 0, 48, 100);     // C triad walk
  setEvent(p4_bass, 0, 8,  true, 48, 0, 48, 100);     // C triad walk

  // ============================================================================
  // PATTERN 5-6: FULL GROOVE - Progression I-V-vi-IV
  // ============================================================================
  for (uint8_t pat = 5; pat <= 6; pat++) {
    Pattern& drums = song.getPattern(1, pat);
    // Four-on-the-floor
    for (uint8_t i = 0; i < 16; i += 4) {
      setEvent(drums, 0, i, true, 127, 64, 50, 64);
    }
    // Snare
    setEvent(drums, 1, 4,  true, 127, 64, 100, 64);
    setEvent(drums, 1, 12, true, 127, 64, 100, 64);
    // Hats with accent
    for (uint8_t i = 0; i < 16; i++) {
      setEvent(drums, 2, i, true, i % 4 == 0 ? 110 : 70, 64, 20, 64);
    }
    // Open hat accents
    setEvent(drums, 3, 6,  true, 90, 64, 200, 64);
    setEvent(drums, 3, 14, true, 85, 64, 200, 64);

    Pattern& arp_pat = song.getPattern(4, pat);
    setEvent(arp_pat, 0, 0,  true, 85, 20, 50, 48);   // C
    setEvent(arp_pat, 0, 4,  true, 92, 20, 50, 48);   // G
    setEvent(arp_pat, 0, 8,  true, 95, 16, 50, 48);   // Am
    setEvent(arp_pat, 0, 12, true, 82, 20, 50, 48);   // F

    Pattern& bass_pat = song.getPattern(5, pat);
    setEvent(bass_pat, 0, 0,  true, 48, 0, 64, 100);  // C jazz walk
    setEvent(bass_pat, 0, 4,  true, 55, 0, 64, 100);  // G jazz walk
    setEvent(bass_pat, 0, 8,  true, 57, 16, 64, 100); // Am jazz walk
    setEvent(bass_pat, 0, 12, true, 53, 0, 64, 100);  // F jazz walk
  }

  // ============================================================================
  // PATTERN 7-8: VARIATION - Chromatic bass
  // ============================================================================
  for (uint8_t pat = 7; pat <= 8; pat++) {
    Pattern& drums = song.getPattern(1, pat);
    for (uint8_t i = 0; i < 16; i += 4) {
      setEvent(drums, 0, i, true, 127, 64, 50, 64);
    }
    setEvent(drums, 1, 4,  true, 127, 64, 100, 64);
    setEvent(drums, 1, 12, true, 127, 64, 100, 64);
    for (uint8_t i = 0; i < 16; i++) {
      setEvent(drums, 2, i, true, i % 4 == 0 ? 110 : 70, 64, 20, 64);
    }

    Pattern& arp_pat = song.getPattern(4, pat);
    setEvent(arp_pat, 0, 0,  true, 85, 20, 40, 80);
    setEvent(arp_pat, 0, 4,  true, 92, 20, 40, 80);
    setEvent(arp_pat, 0, 8,  true, 95, 16, 40, 80);
    setEvent(arp_pat, 0, 12, true, 82, 20, 40, 80);

    Pattern& bass_pat = song.getPattern(5, pat);
    setEvent(bass_pat, 0, 0,  true, 48, 0, 100, 90);  // Chromatic approach
    setEvent(bass_pat, 0, 4,  true, 55, 0, 100, 90);
    setEvent(bass_pat, 0, 8,  true, 57, 16, 100, 90);
    setEvent(bass_pat, 0, 12, true, 53, 0, 100, 90);
  }

  // ============================================================================
  // PATTERN 9: BREAKDOWN - Just bass + minimal kick
  // ============================================================================
  Pattern& p9_drums = song.getPattern(1, 9);
  setEvent(p9_drums, 0, 0,  true, 100, 64, 100, 64);
  setEvent(p9_drums, 0, 8,  true, 100, 64, 100, 64);

  Pattern& p9_bass = song.getPattern(5, 9);
  setEvent(p9_bass, 0, 0,  true, 48, 0, 80, 127);     // Fifth pedal
  setEvent(p9_bass, 0, 8,  true, 48, 0, 80, 127);

  // ============================================================================
  // PATTERN 10: BUILD BACK - Add elements
  // ============================================================================
  Pattern& p10_drums = song.getPattern(1, 10);
  for (uint8_t i = 0; i < 16; i += 4) {
    setEvent(p10_drums, 0, i, true, 120, 64, 50, 64);
  }
  setEvent(p10_drums, 1, 4,  true, 110, 64, 100, 64);
  setEvent(p10_drums, 1, 12, true, 110, 64, 100, 64);
  for (uint8_t i = 8; i < 16; i++) {
    setEvent(p10_drums, 2, i, true, 80, 64, 20, 64);
  }

  Pattern& p10_bass = song.getPattern(5, 10);
  setEvent(p10_bass, 0, 0,  true, 48, 0, 48, 110);
  setEvent(p10_bass, 0, 4,  true, 55, 0, 48, 110);
  setEvent(p10_bass, 0, 8,  true, 57, 16, 48, 110);

  // ============================================================================
  // PATTERN 11-12: BIG FINISH - All elements
  // ============================================================================
  for (uint8_t pat = 11; pat <= 12; pat++) {
    Pattern& drums = song.getPattern(1, pat);
    for (uint8_t i = 0; i < 16; i += 4) {
      setEvent(drums, 0, i, true, 127, 64, 50, 64);
    }
    setEvent(drums, 1, 4,  true, 127, 64, 100, 64);
    setEvent(drums, 1, 12, true, 127, 64, 100, 64);
    for (uint8_t i = 0; i < 16; i++) {
      setEvent(drums, 2, i, true, i % 4 == 0 ? 120 : 80, 64, 20, 64);
    }
    setEvent(drums, 6, 0, true, 127, 64, 500, 64);    // Crash!

    Pattern& arp_pat = song.getPattern(4, pat);
    for (uint8_t i = 0; i < 16; i += 2) {
      uint8_t notes[] = {85, 92, 95, 82};
      uint8_t scales[] = {20, 20, 16, 20};
      setEvent(arp_pat, 0, i, true, notes[i/4], scales[i/4], 40, 96);
    }

    Pattern& bass_pat = song.getPattern(5, pat);
    setEvent(bass_pat, 0, 0,  true, 48, 0, 64, 127);
    setEvent(bass_pat, 0, 4,  true, 55, 0, 64, 127);
    setEvent(bass_pat, 0, 8,  true, 57, 16, 64, 127);
    setEvent(bass_pat, 0, 12, true, 53, 0, 64, 127);
  }
}
