# GRUVBOK Architecture Documentation

## Overview

GRUVBOK is a hardware MIDI groovebox with a unique "always playing" philosophy. The system continuously loops through a data structure that users build in real-time by pressing buttons and moving knobs.

## Core Philosophy

- **Always Playing**: The sequence never stops; it only plays silence when no data is programmed
- **Immediate Feedback**: Button presses and pot movements are instantly recorded and heard
- **Data-Centric**: The "song" is a data structure, not a timeline
- **Modular Interpretation**: Different modes interpret the same data structure in unique ways

## Architecture Layers

### Layer 1: Core Data Structures (`src/core/`)

The foundational data model:

```
Song
  └─ Mode[15] (MIDI channels 1-15)
      └─ Pattern[32] (32 patterns per mode)
          └─ Track[8] (8 parallel tracks)
              └─ Event[16] (16 steps, matching hardware buttons)
```

#### Event (4 bytes, bit-packed)
- **Switch**: 1 bit (on/off)
- **Pot[4]**: 4 × 7 bits (values 0-127)
- **Total**: 29 bits in uint32_t

#### Memory Usage
- Event: 4 bytes
- Track: 64 bytes (16 events)
- Pattern: 512 bytes (8 tracks)
- Mode: 16,384 bytes (32 patterns)
- **Song: 245,760 bytes (~240 KB)**

### Layer 2: Hardware Abstraction (`src/hardware/`)

**Hardware.h/cpp**: Clean interface to physical I/O
- 16 momentary buttons (B1-B16) → digital pins with debouncing
- 4 slider potentiometers (S1-S4) → analog pins with change detection
- 1 LED → visual feedback
- No dependencies on specific pin layout in higher layers

### Layer 3: Sequencer Engine (`src/sequencer/`)

**Sequencer.h/cpp**: The heart of GRUVBOK
- Manages playback state (always playing, always looping)
- Tracks current position (mode, pattern, track, step)
- Handles timing and tempo (BPM → step interval)
- Processes user input → records Events
- Coordinates all modes
- Outputs MIDI clock (24 PPQN)

**MIDIScheduler.h/cpp**: Delta-time MIDI scheduling
- Queue of scheduled MIDI events
- Modes schedule events with delta timing
- Executes events at precise times
- API: `note()`, `off()`, `cc()`, `stopall()`

### Layer 4: Modes (`src/modes/`)

**Mode.h**: Base class for all modes
- Pure virtual `processEvent()` function
- Receives: trackIndex, Event, stepTime
- Returns: MIDI events via scheduler API

**Mode0_PatternSequencer**: Master controller
- Controls which pattern plays on other modes
- Event interpretation:
  - Pot 0: Target pattern (0-31)
  - Pot 1: Transition type (immediate/loop-end/crossfade)
  - Pot 2/3: Mode bitmask (which modes to affect)

**Mode1_DrumMachine**: Classic drum sequencer
- 8 tracks = 8 drum sounds (GM mapping)
- Event interpretation:
  - Switch: Trigger drum hit
  - Pot 0: Velocity
  - Pot 1: Pan (CC10)
  - Pot 2: Tone/Filter (CC74)
  - Pot 3: Reverb (CC91)

## Dataflow

### Recording (User → Data)

```
User presses Button 5
  ↓
Hardware.readButtonPress(5) = true
  ↓
Sequencer.recordEvent(5)
  ↓
Get current Event at step 5
  ↓
Toggle Event.switch
Read all 4 pots → Event.pot[0-3]
  ↓
Event stored in Song data structure
```

### Playback (Data → MIDI)

```
Timer triggers step advancement
  ↓
Sequencer.advanceStep() → currentStep++
  ↓
Sequencer.processStep()
  ↓
For each Mode (0-14):
  For each Track (0-7):
    Get Event at currentStep
    ↓
    Mode.processEvent(track, event)
      ↓
      Mode interprets Event data
      ↓
      Scheduler.note(pitch, velocity, delta)
      Scheduler.off(pitch, delta+length)
      Scheduler.cc(controller, value, delta)
  ↓
MIDIScheduler.update()
  ↓
Execute scheduled events at precise times
  ↓
usbMIDI.sendNoteOn/Off/CC()
```

## Design Principles

### 1. No Dynamic Allocation in Playback Loop
- All memory allocated at startup
- Fixed-size arrays throughout
- Predictable, real-time performance

### 2. Bit-Packing for Memory Efficiency
- Event fits in 4 bytes (32 bits)
- Enables 240KB song data to fit in 1MB RAM
- Room for runtime state and stack

### 3. Separation of Concerns
- Hardware layer: Physical I/O only
- Sequencer: Timing and coordination only
- Modes: Musical interpretation only
- Scheduler: MIDI output only

### 4. Data-Centric Design
- Song is the source of truth
- Modes are stateless interpreters
- Easy to save/load/transmit songs

### 5. Real-Time Constraints
- Update loop must complete quickly (<1ms)
- No blocking operations
- Delta scheduling for precise timing

## Extending GRUVBOK

### Adding a New Mode

1. Create `ModeN_YourMode.h` in `src/modes/`
2. Inherit from `Mode` base class
3. Implement `processEvent()`:
   - Read Event data (switch, pots)
   - Interpret musically
   - Schedule MIDI via `scheduler->note/off/cc()`
4. Register in `Sequencer.init()`:
   ```cpp
   modes[N] = new ModeN_YourMode(N+1);  // MIDI channel N+1
   ```

### Example Mode Template

```cpp
class ModeN_YourMode : public Mode {
public:
  ModeN_YourMode(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event, unsigned long stepTime) override {
    if (!event.getSwitch()) return;  // Only process active events

    // Interpret pots
    uint8_t param1 = event.getPot(0);
    uint8_t param2 = event.getPot(1);

    // Generate MIDI
    scheduler->note(midiChannel, 60 + trackIndex, 100, 0);
    scheduler->off(midiChannel, 60 + trackIndex, 100);
  }

  const char* getName() const override { return "YourMode"; }
};
```

## Testing

Unit tests use Unity framework:
- `test/test_event.cpp`: Event data structure
- `test/test_track.cpp`: Track container
- More tests can be added for Pattern, Song, etc.

Run tests:
```bash
pio test
```

## Memory Map

```
Teensy 4.1 (1MB RAM):
- Song data:        ~240 KB
- MIDIScheduler:    ~4 KB (64 event slots)
- Sequencer state:  ~1 KB
- Mode instances:   ~1 KB
- Stack/heap:       ~50 KB
- Available:        ~700 KB
```

## Future Enhancements

- [ ] Song save/load to SD card
- [ ] MIDI input handling (sync to external clock)
- [ ] More modes (Modes 2-14)
- [ ] Pattern chaining
- [ ] Swing/groove quantization
- [ ] Per-track length (polyrhythms)
- [ ] MIDI learn for CC mapping

## Performance Targets

- Update loop: <1ms typical, <5ms worst-case
- MIDI latency: <1ms (USB MIDI)
- Step timing jitter: <1ms
- Button debounce: 20ms
