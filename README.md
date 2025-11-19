# GRUVBOK - Hardware MIDI Groovebox

A modular, always-playing musical instrument built on Teensy 4.1 with pure functional architecture.

## Overview

GRUVBOK is a hardware sequencer with a unique dataflow-oriented design:
- **Always playing, always looping** - no start/stop states
- **Pure functional modes** - transform data to MIDI with zero side effects
- **Mode-agnostic recording** - hardware captures state into data
- **Modular architecture** - easy to extend with new modes

## Hardware

- **Teensy 4.1** microcontroller
- **16 buttons** - program sequencer steps
- **8 potentiometers** - 4 navigation, 4 parameters
- **1 LED** - step indicator
- **USB MIDI** output

## Quick Start

```bash
# Build and upload
pio run -t upload

# Run tests
pio test

# See docs/QUICKSTART.md for detailed setup
```

## Documentation

- **[Quickstart Guide](docs/QUICKSTART.md)** - Get up and running
- **[Architecture Overview](docs/ARCHITECTURE.md)** - System design
- **[Pure Functional Refactor](docs/PURE_FUNCTIONAL_REFACTOR.md)** - Why modes are truly pure
- **[Code Review](docs/CODE_REVIEW.md)** - Type safety and testing improvements
- **[Default Songs](docs/DEFAULT_SONG_INFO.md)** - Pre-programmed patterns

## Key Features

✅ **15 Modes** - Different musical behaviors (drum machine, etc.)
✅ **32 Patterns** per mode - Song arrangement
✅ **8 Tracks** per pattern - Parallel voices
✅ **16 Steps** per track - Sequencer length
✅ **Pure functional** - Modes have zero side effects
✅ **240KB song data** - Entire composition in memory
✅ **Real-time MIDI** - Sub-millisecond timing

## Architecture Highlights

**Pure Functional Modes:**
```cpp
// Modes transform data → data (no I/O, no side effects)
void Mode::processEvent(Event input, MIDIEventBuffer& output) const {
    output.noteOn(channel, note, velocity, 0);  // Pure data
}
```

**Clean Dataflow:**
```
Hardware → Event → Mode → MIDIEvents → Scheduler → MIDI Hardware
  (input)   (data)  (pure)   (data)      (I/O)      (output)
```

## Building

Requires [PlatformIO](https://platformio.org/):

```bash
# Install dependencies
pio pkg install

# Build
pio run

# Upload to Teensy
pio run -t upload

# Run unit tests (57 tests)
pio test
```

## Project Structure

```
src/
├── core/          # Data structures (Event, Track, Pattern, Song)
├── hardware/      # Hardware I/O abstraction
├── modes/         # Musical interpreters (pure functions)
└── sequencer/     # Timing and MIDI scheduling

test/              # Comprehensive unit tests
docs/              # Documentation
```

## License

MIT

## Credits

Built with pure functional principles and careful attention to real-time constraints.

See [CLAUDE.md](CLAUDE.md) for detailed project instructions and philosophy.
