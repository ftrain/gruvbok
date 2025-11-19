# GRUVBOK Default Song

## Overview

GRUVBOK now loads a demo drum pattern automatically on power-up, so there's **something to hear immediately**! This demonstrates the "always playing" philosophy and showcases the drum machine capabilities.

## Default Demo Pattern

The default song is a **classic house/dance beat** on Mode1 (Drum Machine):

### Track Layout

**Track 0 - Kick Drum (C1/36)**
- Four-on-the-floor pattern
- Steps: 0, 4, 8, 12 (every beat)
- Velocity: Strong on 1, slightly softer on 2-4

**Track 1 - Snare Drum (D1/38)**
- Backbeat on 2 and 4
- Steps: 4, 12
- With reverb (pot 3 = 20)

**Track 2 - Closed Hi-Hat (F#1/42)**
- 8th note pattern
- Steps: 0, 2, 4, 6, 8, 10, 12, 14
- Accented on beats (every 4 steps)

**Track 3 - Open Hi-Hat (A#1/46)**
- Sparse accents for groove
- Steps: 6, 14 (between beats)
- Light reverb

**Track 4 - Low Tom (G1/43)**
- Fill at end of loop
- Step: 15 (leads back to step 0)

**Track 6 - Crash Cymbal (C#2/49)**
- Emphasis on the downbeat
- Step: 0
- Heavy reverb (pot 3 = 30)

## Pattern Visualization

```
Step:  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
       |   .   .   .   |   .   .   .   |   .   .   .   |   .   .   .
Kick:  X   -   -   -   X   -   -   -   X   -   -   -   X   -   -   -
Snare: -   -   -   -   X   -   -   -   -   -   -   -   X   -   -   -
ClHat: X   -   X   -   X   -   X   -   X   -   X   -   X   -   X   -
OpHat: -   -   -   -   -   -   X   -   -   -   -   -   -   -   X   -
LTom:  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   X
Crash: X   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

Beat:  1       2       3       4
```

## Additional Presets

You can load different patterns by calling these functions in `main.cpp`:

### Techno Pattern
```cpp
DefaultSongs::loadTechnoPattern(song);
```
- Constant four-on-the-floor kick
- 16th note hi-hats with filter (CC74)
- Sparse claps with reverb
- Driving techno energy

### Breakbeat Pattern
```cpp
DefaultSongs::loadBreakbeat(song);
```
- Funky, syncopated kick placement
- Classic breakbeat snare pattern
- Complex 16th note hi-hat groove
- Fill toms at strategic points

### Empty Song
```cpp
DefaultSongs::loadEmpty(song);
```
- Clears all patterns
- Silence (for starting from scratch)

## Customizing the Default

To change which pattern loads on startup, edit `src/main.cpp`:

```cpp
void setup() {
  hardware.init();

  // Choose your default:
  DefaultSongs::loadDemoSong(song);      // Current default
  // DefaultSongs::loadTechnoPattern(song);  // Uncomment for techno
  // DefaultSongs::loadBreakbeat(song);      // Uncomment for breaks
  // DefaultSongs::loadEmpty(song);          // Uncomment for silence

  sequencer.init();
  sequencer.setBPM(120.0);
  sequencer.start();
}
```

## MIDI Mapping (GM Drums)

When connected to a GM-compatible drum synth or DAW:

| Track | Drum Sound      | MIDI Note | GM Standard |
|-------|-----------------|-----------|-------------|
| 0     | Kick Drum       | 36 (C1)   | Bass Drum 1 |
| 1     | Snare Drum      | 38 (D1)   | Acoustic Snare |
| 2     | Closed Hi-Hat   | 42 (F#1)  | Closed Hi-Hat |
| 3     | Open Hi-Hat     | 46 (A#1)  | Open Hi-Hat |
| 4     | Low Tom         | 43 (G1)   | Low Floor Tom |
| 5     | Mid Tom         | 47 (B1)   | Low-Mid Tom |
| 6     | Crash Cymbal    | 49 (C#2)  | Crash Cymbal 1 |
| 7     | Ride Cymbal     | 51 (D#2)  | Ride Cymbal 1 |

## Live Editing

The default song is just a starting point! You can:

1. **Toggle steps**: Press buttons 1-16 to turn steps on/off
2. **Adjust tempo**: Turn Pot 0 (60-240 BPM)
3. **Modify parameters**: Turn pots while pressing buttons to capture new values
   - Pot 0: Velocity (0-127)
   - Pot 1: Pan (CC10)
   - Pot 2: Filter/Tone (CC74)
   - Pot 3: Reverb Send (CC91)

All changes are **immediately audible** - this is the GRUVBOK way!

## Implementation Details

### Code Location
- `src/core/DefaultSongs.h` - Function declarations
- `src/core/DefaultSongs.cpp` - Pattern implementations
- `src/main.cpp` - Loads default on startup

### Memory Impact
Default songs are loaded once at startup and stored in the same Song data structure. No additional memory overhead beyond the already-allocated 240KB.

### Creating Your Own Presets

Add new preset functions to `DefaultSongs.cpp`:

```cpp
void DefaultSongs::loadMyPattern(Song& song) {
  song.clear();
  Pattern& drums = song.getPattern(1, 0);  // Mode1, Pattern 0

  // Track 0, Step 0: Kick with max velocity
  setEvent(drums, 0, 0, true, 127, 64, 0, 0);

  // Track 1, Step 4: Snare with reverb
  setEvent(drums, 1, 4, true, 120, 64, 0, 30);

  // ... more events ...
}
```

Then call it from `setup()` in `main.cpp`.

## Testing

Connect GRUVBOK to:
1. **DAW** with GM drums (Logic, Ableton, FL Studio)
2. **Hardware drum machine** that responds to MIDI
3. **Software synth** (Addictive Drums, Superior Drummer)

Set the input to **MIDI channel 2** (Mode1's channel).

You should hear the demo beat playing immediately at 120 BPM, with the LED blinking on every beat.

## Philosophy

The default song embodies GRUVBOK's core philosophy:
- **Always Playing**: Never silent, always in motion
- **Immediate Gratification**: Sounds good right out of the box
- **Editable**: Starting point for creativity, not a constraint
- **Demonstrative**: Shows what's possible with the hardware

Press buttons, turn knobs, and **make it yours**! 🎵
