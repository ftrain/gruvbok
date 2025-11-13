#ifndef MODE0_PATTERNSEQUENCER_H
#define MODE0_PATTERNSEQUENCER_H

#include "Mode.h"

/**
 * Mode0 - Pattern Sequencer (Master Control)
 *
 * Mode 0 is special: it controls which pattern plays on all other modes.
 * Instead of generating MIDI, it stores pattern selection commands.
 *
 * Event interpretation:
 * - Switch: Pattern change trigger
 * - Pot 0: Target pattern number (0-31)
 * - Pot 1: Transition type (0=immediate, 64=end-of-loop, 127=crossfade)
 * - Pot 2: Target mode bitmask low (bits 0-6)
 * - Pot 3: Target mode bitmask high (bit 7 + future expansion)
 *
 * When Switch is active, the sequencer engine reads the pots to determine
 * which pattern to switch to and on which modes.
 *
 * Example: Switch=1, Pot0=5, Pot1=0, Pot2=127, Pot3=1
 * Result: Immediately switch to pattern 5 on modes 0-7 and mode 8
 */
class Mode0_PatternSequencer : public Mode {
public:
  Mode0_PatternSequencer(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event, unsigned long stepTime) override {
    // Mode 0 doesn't generate MIDI directly
    // The Sequencer engine reads events from Mode 0 to control pattern playback
    // No MIDI scheduling needed here
  }

  /**
   * Helper: Get target pattern from event
   */
  static uint8_t getTargetPattern(const Event& event) {
    // Map pot value (0-127) to pattern (0-31)
    uint8_t potValue = event.getPot(0);
    return (potValue * 32) / 128;
  }

  /**
   * Helper: Get transition type from event
   */
  enum TransitionType {
    IMMEDIATE = 0,
    END_OF_LOOP = 1,
    CROSSFADE = 2
  };

  static TransitionType getTransitionType(const Event& event) {
    uint8_t potValue = event.getPot(1);
    if (potValue < 43) return IMMEDIATE;
    if (potValue < 85) return END_OF_LOOP;
    return CROSSFADE;
  }

  /**
   * Helper: Get mode bitmask (which modes to affect)
   */
  static uint16_t getModeBitmask(const Event& event) {
    uint8_t low = event.getPot(2);
    uint8_t high = event.getPot(3);
    return ((uint16_t)high << 7) | low;
  }

  const char* getName() const override {
    return "PatternSeq";
  }
};

#endif // MODE0_PATTERNSEQUENCER_H
