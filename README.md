# GRUVBOK

**A modular, always-playing hardware MIDI groovebox controller built on Teensy 4.1**

GRUVBOK is a hardware sequencer and MIDI controller that combines real-time performance controls with multiple musical modes - from drum machines to arpeggios, bass sequencers, and generative patterns. Built with a clean, modular C++ architecture that makes it easy to add new modes and extend functionality.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Teensy%204.1-orange.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-brightgreen.svg)

## Features

- **6 Musical Modes** (with room for 9 more):
  - Mode 0: Pattern Sequencer (song arrangement)
  - Mode 1: Drum Machine (8 GM percussion voices)
  - Mode 2: Acid Bass (TB-303 style with portamento)
  - Mode 3: Euclidean Fade (generative echo patterns)
  - Mode 4: Meta-Arp (directional scale arpeggios)
  - Mode 5: Bassline Progression (walking bass with 8 styles)

- **Always Playing**: Continuous sequencer that never stops, just loops
- **16 Steps × 8 Tracks × 32 Patterns × 15 Modes** = massive creative space
- **Real-time Control**: 4 navigation pots + 4 parameter sliders + 16 buttons
- **USB MIDI**: Plug and play with your DAW or VCV Rack
- **Modular Architecture**: Pure functional design makes adding modes simple
- **Memory Efficient**: Entire song fits in 240KB RAM

## Quick Start

### Hardware Requirements

- **Teensy 4.1** microcontroller
- **16 momentary buttons** (for step programming)
- **4 potentiometers** (navigation: tempo, mode, pattern, track)
- **4 slider potentiometers** (parameters: varies by mode)
- **1 LED** (step indicator, optional)
- USB cable for MIDI and power

### Software Setup

1. **Install PlatformIO**:
   ```bash
   # Install PlatformIO CLI
   pip install platformio
   ```

2. **Clone and Build**:
   ```bash
   git clone https://github.com/yourusername/gruvbok.git
   cd gruvbok
   pio run --target upload
   ```

3. **Connect to Your DAW**:
   - GRUVBOK appears as "Teensy MIDI"
   - Receives on 15 MIDI channels (one per mode)
   - Sends debug CCs on channel 16 for monitoring

### Basic Operation

**Navigation (always active)**:
- **Pot 0**: Tempo (20-800 BPM)
- **Pot 1**: Mode Select (0-14)
- **Pot 2**: Pattern Select (0-31)
- **Pot 3**: Track Select (0-7)

**Programming**:
1. Select mode/pattern/track with navigation pots
2. Adjust sliders to set parameters
3. Press button 1-16 to toggle that step ON/OFF
4. Step captures current slider values when toggled

**Pre-loaded Song**:
- Pattern 0: Silent (for testing)
- Patterns 1-12: Complete arranged song (intro → build → groove → breakdown → finale)
- Turn Pot 2 to navigate patterns

## Architecture

### Data Model
```
Song (240KB)
└─ Mode[15]           // 15 simultaneous MIDI channels
   └─ Pattern[32]     // 32 patterns per mode
      └─ Track[8]     // 8 voices/instruments per pattern
         └─ Event[16] // 16 steps per track
            └─ {switch: bool, pots[4]: 0-127}
```

### Core Components

- **Hardware** (`src/hardware/`): Button/pot reading, debouncing
- **Song** (`src/core/`): Bit-packed data structure (4 bytes per event)
- **Sequencer** (`src/sequencer/`): Timing engine, always-playing loop
- **MIDIScheduler** (`src/sequencer/`): Delta-time event scheduling
- **Modes** (`src/modes/`): Musical interpreters (pure functions, no side effects)

### Adding a New Mode

1. Create `src/modes/ModeN_YourMode.h`
2. Inherit from `Mode` base class
3. Implement `processEvent()` to transform Event → MIDI
4. Register in `Sequencer::init()`

Example:
```cpp
class Mode6_YourMode : public Mode {
public:
  Mode6_YourMode(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (!event.getSwitch()) return;

    // Read parameters from event
    uint8_t pitch = event.getPot(0);
    uint8_t velocity = event.getPot(1);

    // Generate MIDI
    output.noteOn(midiChannel, pitch, velocity, 0);
    output.noteOff(midiChannel, pitch, 100);  // 100ms later
  }

  const char* getName() const override { return "YourMode"; }
};
```

See `docs/DEVELOPMENT.md` for detailed mode development guide.

## Documentation

- **[DEVELOPMENT.md](docs/DEVELOPMENT.md)** - Detailed development guide
- **[AI_DEVELOPMENT.md](docs/AI_DEVELOPMENT.md)** - Using LLMs to extend GRUVBOK
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - How to contribute
- **[HARDWARE.md](docs/HARDWARE.md)** - Hardware build guide and schematics
- **[API.md](docs/API.md)** - Complete API reference

## Project Structure

```
gruvbok/
├── src/
│   ├── main.cpp              # Entry point
│   ├── core/                 # Data structures
│   │   ├── Event.h           # 4-byte bit-packed event
│   │   ├── Track.h           # 16 events
│   │   ├── Pattern.h         # 8 tracks
│   │   ├── Song.h            # 15 modes × 32 patterns
│   │   └── DefaultSongs.cpp  # Factory presets
│   ├── hardware/             # Hardware abstraction
│   │   ├── Hardware.h/cpp    # I/O layer
│   │   └── InputState.h      # Input snapshot
│   ├── sequencer/            # Timing and playback
│   │   ├── Sequencer.h/cpp   # Main engine
│   │   └── MIDIScheduler.h/cpp
│   └── modes/                # Musical modes
│       ├── Mode.h            # Base class
│       ├── Mode0_PatternSequencer.h
│       ├── Mode1_DrumMachine.h
│       ├── Mode2_AcidBass.h
│       ├── Mode3_EuclideanFade.h
│       ├── Mode4_MetaArp.h
│       └── Mode5_BasslineProgression.h
├── hardware.ini              # Pin mappings
├── platformio.ini            # Build configuration
├── docs/                     # Documentation
└── LICENSE                   # MIT License
```

## Use with VCV Rack

GRUVBOK works great as a hardware controller for VCV Rack:

1. **MIDI-CV Module**: Connect to "Teensy MIDI" device
2. **Monitor Mode**: Use MIDI-Map on Channel 16 to see:
   - CC1: Current mode (0-14)
   - CC2: Current pattern (0-31)
   - CC3: Current track (0-7)
3. **Receive Notes**: Each mode sends on its own channel (1-15)

## Performance

- **Memory**: 240KB song data + ~50KB code
- **Timing**: Sub-millisecond step accuracy
- **MIDI**: Delta-time scheduling prevents jitter
- **Loop Frequency**: ~10kHz (100μs per iteration)

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code style guidelines
- How to submit PRs
- Testing requirements
- Documentation standards

## AI-Assisted Development

GRUVBOK was designed to be easily extensible with LLM assistance. See [docs/AI_DEVELOPMENT.md](docs/AI_DEVELOPMENT.md) for:
- How to use Claude/ChatGPT to add new modes
- Prompt templates for mode generation
- Architecture patterns that work well with LLMs
- Testing and validation workflows

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Credits

Built with:
- [PlatformIO](https://platformio.org/) - Build system
- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) - Hardware platform
- [MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library) - MIDI protocol

Developed with assistance from Claude (Anthropic) - demonstrating how LLMs can accelerate embedded systems development.

## Community

- **Issues**: Report bugs or request features via GitHub Issues
- **Discussions**: Share your modes, patterns, and builds
- **Pull Requests**: Contributions welcome!

---

**Make music, write code, repeat.** 🎹🎛️
