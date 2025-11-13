# GRUVBOK - Hardware MIDI Groovebox

A modular, always-playing musical instrument built on Teensy 4.1.

## Philosophy

**Always Playing, Always Looping**
- The sequencer never stops - it continuously loops through the song data
- Hardware inputs capture state into the data structure
- Modes are pure functions that transform stored data into MIDI events
- Clean separation: recording (hardware → data) and playback (data → MIDI)

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│ RECORDING: Hardware → Song Data                         │
└─────────────────────────────────────────────────────────┘

Hardware.getCurrentState() → InputState (snapshot of all controls)
                                    ↓
Button press → Event.toggleSwitch() + capture sliders → Event.pots[0-3]
                                    ↓
                              Song data updated


┌─────────────────────────────────────────────────────────┐
│ PLAYBACK: Song Data → MIDI (always looping)             │
└─────────────────────────────────────────────────────────┘

Timer advances step (based on BPM)
         ↓
Song.getEvent(mode, pattern, track, step) → Event (switch + 4 pot values)
         ↓
Mode.processEvent(trackIndex, event, stepTime)  ← Pure function
         ↓
MIDIScheduler → MIDI events with delta timing
```

## Data Structure

**Song**: 240 KB total
- 15 Modes (different MIDI channels)
- 32 Patterns per mode (song arrangement)
- 8 Tracks per pattern (instruments/voices)
- 16 Events per track (sequencer steps)

**Event**: 4 bytes (bit-packed)
- 1 bit: switch (on/off)
- 4×7 bits: pot values (0-127 each)

**Memory efficient**: Event struct is only 4 bytes, entire song fits in 240KB

## Hardware

**16 Buttons**: Toggle events at steps 0-15
**4 Pots** (navigation):
- Pot 0: Tempo (20-800 BPM, centered at 120)
- Pot 1: Mode selection (0-14)
- Pot 2: Pattern selection (0-31)
- Pot 3: Track selection (0-7)

**4 Sliders** (programming):
- Slider 0: Parameter 0 (mode-dependent)
- Slider 1: Parameter 1 (mode-dependent)
- Slider 2: Parameter 2 (mode-dependent)
- Slider 3: Parameter 3 (mode-dependent)

**1 LED**: Step indicator (PWM, bright on step 0, dim on others)

## Modes

Modes are pure interpreters: they transform Event data → MIDI events.

**Mode 0**: Pattern Sequencer (master control)
- Controls which patterns play on other modes
- Meta-sequencer for song arrangement

**Mode 1**: Drum Machine (MIDI channel 2)
- 8 tracks → 8 GM drum sounds
- Slider 0 → Velocity
- Slider 1 → Flam (grace note delay, 0-50ms)
- Slider 2 → Note length (10ms-2000ms)
- Slider 3 → Pan (stereo position)

Track mapping:
- Track 0: Kick (C1/36)
- Track 1: Snare (D1/38)
- Track 2: Closed Hi-Hat (F#1/42)
- Track 3: Open Hi-Hat (A#1/46)
- Track 4: Low Tom (G1/43)
- Track 5: Mid Tom (B1/47)
- Track 6: Crash (C#2/49)
- Track 7: Ride (D#2/51)

**Mode 2-14**: Not yet implemented

## Core Components

**Hardware** (`src/hardware/`)
- Hardware abstraction layer
- Reads buttons, pots, sliders
- Returns `InputState` snapshot (pure data)
- Exponential moving average smoothing on analog inputs

**Song** (`src/core/`)
- Data structure: Mode[15] → Pattern[32] → Track[8] → Event[16]
- Bit-packed Event storage (4 bytes)
- No logic, just data

**Sequencer** (`src/sequencer/`)
- Timing engine (BPM, step advancement)
- Orchestrates recording and playback
- Delegates to modes (no mode-specific logic)
- Manages MIDI clock (24 PPQN)

**MIDIScheduler** (`src/sequencer/`)
- Delta-time MIDI event scheduling
- Queues note on/off and CC messages
- Executes events at precise times

**Mode** (`src/modes/`)
- Base class with single pure function: `processEvent()`
- Each mode interprets Event data differently
- Mode 0: Pattern control
- Mode 1: Drum machine
- Future modes: Acid bass, chord sequencer, etc.

## Dataflow Principles

**Recording is mode-agnostic**:
- Press button → toggle switch + capture slider values
- No mode-specific interpretation
- Modes don't know about recording

**Playback is mode-specific**:
- Mode receives Event with 4 raw pot values (0-127)
- Mode interprets values as it sees fit
- Mode 1: velocity/flam/length/pan
- Mode 2 (future): pitch/resonance/cutoff/accent
- Modes are pure functions (no side effects, no state)

**Clean separation**:
- Hardware layer: physical I/O → InputState
- Recording: InputState → Song data (mode-agnostic)
- Playback: Song data → MIDI (mode-specific)
- No coupling between recording and playback

## Workflow Example: Programming a Drum Beat

1. **Navigate**: Turn Pot 3 to select Track 0 (kick)
2. **Set parameters**: Move sliders to desired velocity/flam/length/pan
3. **Program**: Press Button 1 (step 0) → kick plays on beat 1
4. **Record more**: Press Buttons 5, 9, 13 → four-on-the-floor
5. **Switch tracks**: Turn Pot 3 to Track 1 (snare)
6. **Adjust**: Set different slider values
7. **Program**: Press Buttons 5, 13 → backbeat
8. **Play**: Pattern loops continuously with your programmed beats

Each button press captures the current slider state into that step's event.
Each step can have different parameters!

## MIDI Debug Output

**Channel 16** (navigation):
- CC1: Current mode (0-14)
- CC2: Current pattern (0-31)
- CC3: Current track (0-7)

**Channel 2** (drum machine sliders):
- CC20: Velocity (0-127)
- CC21: Flam (0-127)
- CC22: Length (0-127)
- CC23: Pan (0-127)

## Building

```bash
# Build
pio run

# Build and upload
pio run --target upload

# Run tests
pio test
```

## File Structure

```
src/
├── main.cpp                   # Entry point, setup/loop
├── core/                      # Data structures
│   ├── Event.h                # Bit-packed event (4 bytes)
│   ├── Track.h                # 16 events
│   ├── Pattern.h              # 8 tracks
│   ├── Song.h                 # 15 modes × 32 patterns
│   ├── DefaultSongs.h/cpp     # Pre-programmed patterns
├── hardware/                  # Hardware abstraction
│   ├── Hardware.h/cpp         # I/O layer
│   ├── InputState.h           # Input snapshot struct
├── sequencer/                 # Timing and scheduling
│   ├── Sequencer.h/cpp        # Main engine
│   ├── MIDIScheduler.h/cpp    # Delta-time MIDI
└── modes/                     # Musical interpreters
    ├── Mode.h                 # Base class
    ├── Mode0_PatternSequencer.h
    └── Mode1_DrumMachine.h

test/                          # Unit tests
├── test_event.cpp
└── test_track.cpp

hardware.ini                   # Pin mappings and config
platformio.ini                 # Build configuration
```

## Adding a New Mode

1. Create `src/modes/ModeN_YourMode.h`
2. Inherit from `Mode` base class
3. Implement `processEvent()` - interpret Event pots however you want
4. Register in `Sequencer::init()`

Example:
```cpp
class Mode2_AcidBass : public Mode {
public:
  Mode2_AcidBass(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event, unsigned long stepTime) override {
    if (!event.getSwitch()) return;

    // Interpret pots for acid bass
    uint8_t pitch = event.getPot(0);      // 0-127 → MIDI note
    uint8_t resonance = event.getPot(1);  // 0-127 → CC71
    uint8_t cutoff = event.getPot(2);     // 0-127 → CC74
    uint8_t accent = event.getPot(3);     // 0-127 → velocity boost

    uint8_t note = 36 + (pitch / 2);  // Map to bass range
    uint8_t velocity = 80 + (accent / 3);

    scheduler->note(midiChannel, note, velocity, 0);
    scheduler->off(midiChannel, note, 100);
    scheduler->cc(midiChannel, 71, resonance, 0);  // Resonance
    scheduler->cc(midiChannel, 74, cutoff, 0);     // Cutoff
  }

  const char* getName() const override { return "AcidBass"; }
};
```

No changes to recording logic needed - sliders automatically work!

## Design Goals Achieved

✅ **Modular**: Adding modes doesn't require changing sequencer code
✅ **Dataflow-oriented**: Clear path from hardware → data → MIDI
✅ **Always playing**: Continuous loop, no start/stop states
✅ **Pure functions**: Modes have no side effects or mutable state
✅ **Memory efficient**: 240KB for entire song, 4 bytes per event
✅ **Scalable**: 15 modes × 32 patterns × 8 tracks × 16 steps
✅ **Real-time**: Sub-millisecond timing with delta scheduling
✅ **Testable**: Core data structures have unit tests

## Performance

- **Memory**: 240KB song data + ~50KB code
- **Timing**: Step advancement accurate to 1ms
- **MIDI**: Delta-time scheduling prevents jitter
- **ADC**: Exponential smoothing filters noise (alpha=0.3)
- **Debounce**: 20ms button debounce prevents double-triggers

## Future Enhancements

- [ ] Mode 2: Acid bass sequencer
- [ ] Mode 3: Chord sequencer
- [ ] Mode 4: Arpeggiator
- [ ] Save/load songs to SD card (LittleFS)
- [ ] Mode 0 pattern chaining (song mode)
- [ ] Per-track mute/solo
- [ ] MIDI input (sync to external clock)
- [ ] Display (OLED for visual feedback)

## License

MIT

## Credits

Built with Claude Code - a modular architecture designed for musical expression.
