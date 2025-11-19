# GRUVBOK Quick Start Guide

## Building & Uploading

```bash
# Build only
pio run

# Build and upload to Teensy
pio run --target upload

# Run tests
pio test
```

## Using the Device

### Current State (Mode1: Drum Machine)

**🎵 NEW: Default Song Plays on Startup!**
GRUVBOK now loads a demo drum beat automatically, so you'll hear music immediately when you power it on. It's a classic house/dance pattern with kick, snare, hi-hats, and crash. See `DEFAULT_SONG_INFO.md` for details.

**Controls:**
- **Buttons 1-16**: Toggle steps in the sequence (lights up when active)
- **Pot 0**: Tempo control (60-240 BPM)
- **LED**: Blinks on every beat (4 steps)

**Operation:**
1. Power on - **demo song starts playing automatically** at 120 BPM
2. Press buttons to toggle steps in the drum pattern
3. Turn Pot 0 to change tempo
4. Pattern loops continuously on 16 steps

**MIDI Output:**
- **Notes**: Multiple drum sounds (GM standard mapping)
- **Channel**: 2 (Mode1)
- **Clock**: 24 PPQN continuously
- **Velocity**: Varies by track (80-127)

### Editing Patterns

**Current Implementation:**
- Only Mode 1, Pattern 0, Track 0 is active
- Button press toggles Event.switch at that step
- All pots are captured (but only Pot 0 controls tempo globally)

**Future**: Use mode/track selection to access other patterns

## Development

### Adding a New Mode

1. Create `src/modes/ModeN_YourName.h`:

```cpp
#ifndef MODEN_YOURNAME_H
#define MODEN_YOURNAME_H

#include "Mode.h"

class ModeN_YourName : public Mode {
public:
  ModeN_YourName(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime) override {
    // Only process active events
    if (!event.getSwitch()) return;

    // Read pot values
    uint8_t velocity = event.getPot(0);
    uint8_t pitch = event.getPot(1);

    // Send MIDI
    uint8_t note = 60 + pitch / 10;  // Map pot to pitch
    scheduler->note(midiChannel, note, velocity, 0);
    scheduler->off(midiChannel, note, 100);  // 100ms later
  }

  const char* getName() const override {
    return "YourName";
  }
};

#endif
```

2. Register in `src/sequencer/Sequencer.cpp`:

```cpp
void Sequencer::init() {
  modes[0] = new Mode0_PatternSequencer(1);
  modes[1] = new Mode1_DrumMachine(2);
  modes[N] = new ModeN_YourName(N+1);  // Add this line
  // ...
}
```

3. Rebuild and upload

### Understanding the Data Flow

#### Button Press → Recording
```
User presses Button 5 (step 5)
  ↓
Hardware::readButtonPress(5) detects press
  ↓
Sequencer::recordEvent(5) called
  ↓
Get Event at [currentMode][currentPattern][currentTrack][step 5]
  ↓
Event::toggleSwitch() - flip on/off
Event::setPot(0-3) - capture all pot values
  ↓
Event stored in Song data structure
```

#### Step Playback → MIDI
```
millis() >= lastStepTime + stepInterval
  ↓
Sequencer::advanceStep() - currentStep++
  ↓
Sequencer::processStep()
  ↓
For each active Mode:
  Get Pattern from Song
  For each Track in Pattern:
    Get Event at currentStep
      ↓
    Mode::processEvent(track, event, time)
      ↓
    Mode interprets Event and schedules MIDI
    scheduler->note(ch, pitch, vel, delta)
      ↓
MIDIScheduler stores event + executeTime
  ↓
MIDIScheduler::update() checks executeTime
  ↓
usbMIDI.sendNoteOn(pitch, vel, channel)
```

## Architecture Overview

```
┌─────────────────────────────────────────────┐
│  main.cpp                                   │
│  - Initialize hardware, sequencer, modes    │
│  - Main loop: sequencer.update()            │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│  Sequencer (Engine)                         │
│  - Timing & step advancement                │
│  - User input handling                      │
│  - Mode coordination                        │
└──┬────────────┬─────────────┬───────────────┘
   │            │             │
   ▼            ▼             ▼
┌──────┐   ┌─────────┐   ┌──────────────┐
│ Song │   │Hardware │   │MIDIScheduler │
│ Data │   │ (I/O)   │   │ (Output)     │
└──────┘   └─────────┘   └──────────────┘
   │
   ▼
Mode0, Mode1, ... Mode14
(Musical Interpreters)
```

## Memory Map

```
Song Structure:
Song
 ├─ Mode[15]
 │   ├─ Pattern[32]
 │   │   ├─ Track[8]
 │   │   │   └─ Event[16]
 │   │   │       ├─ switch (1 bit)
 │   │   │       └─ pot[4] (28 bits)

Memory Usage:
- Event: 4 bytes
- Track: 64 bytes (16 × 4)
- Pattern: 512 bytes (8 × 64)
- Mode: 16 KB (32 × 512)
- Song: 240 KB (15 × 16K)
```

## Hardware Pinout

```
Buttons (INPUT_PULLUP):
B1-B8:   23, 22, 21, 20, 19, 18, 17, 16
B9-B16:  9, 8, 7, 6, 5, 4, 3, 2

Potentiometers (ANALOG):
S1-S4:   24, 25, 26, 27

LED (OUTPUT):
LED:     13 (onboard)
```

## Troubleshooting

### Build fails with "size_t not found"
- Fixed: `Song.h` includes `<stddef.h>`

### Button press not registering
- Check debounce timing (20ms default)
- Verify pull-up resistors enabled
- Test with `Hardware::readButtonState()` for immediate read

### No MIDI output
- Verify USB MIDI device in DAW/OS
- Check MIDI channel (Mode1 = channel 2)
- Confirm Event.switch is true for active steps

### Tempo not changing
- Pot 0 must change by >2 to avoid jitter
- Range: 60-240 BPM
- Check `Hardware::readPotChange()` threshold

### LED not blinking
- LED blinks every 4 steps (quarter notes)
- Verify `currentStep % 4 == 0` logic
- Check pin 13 not used elsewhere

## Next Steps

1. **Test hardware**: Press buttons, verify MIDI output
2. **Add Mode 2**: Implement acid sequencer or chord mode
3. **Implement Mode0**: Enable pattern switching
4. **Add UI**: Track/mode selection, visual feedback
5. **Save/Load**: Persist songs to SD card

## Resources

- `ARCHITECTURE.md` - Detailed architecture documentation
- `REFACTORING_SUMMARY.md` - What changed and why
- `Claude.md` - Original specification
- `src/modes/Mode1_DrumMachine.h` - Example mode implementation

## Support

- Check build output: `pio run -v` (verbose)
- Monitor serial: `pio device monitor`
- Test components: `pio test`
- Verify memory: Check teensy_size output after build
