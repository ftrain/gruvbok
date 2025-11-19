# AI-Assisted Development Guide for GRUVBOK

This guide explains how to use Large Language Models (LLMs) like Claude or ChatGPT to extend GRUVBOK with new modes, features, and functionality.

## Why GRUVBOK Works Well with LLMs

GRUVBOK's architecture was designed to be LLM-friendly:

1. **Pure Functional Modes**: Each mode is a self-contained class with a single `processEvent()` function
2. **Clear Separation of Concerns**: Hardware, sequencing, and musical logic are isolated
3. **Consistent Patterns**: All modes follow the same structure and API
4. **Well-Documented**: Inline comments and documentation make code understandable
5. **Incremental Development**: You can add one mode at a time without touching existing code

## Quick Start: Adding a Mode with an LLM

### Step 1: Provide Context

Give your LLM this project context:

```
I'm working on GRUVBOK, a hardware MIDI sequencer built on Teensy 4.1.
It has a modular architecture where musical "modes" are pure functions
that transform event data into MIDI messages.

Key files to understand:
- src/modes/Mode.h - Base class for all modes
- src/modes/Mode1_DrumMachine.h - Example simple mode
- src/modes/Mode4_MetaArp.h - Example complex mode
- src/core/Event.h - Data structure (4 bytes: switch + 4 pots)
- src/sequencer/MIDIScheduler.h - MIDI output API

Each mode receives events with:
- event.getSwitch() - bool, is this step active?
- event.getPot(0-3) - uint8_t (0-127), 4 parameters per step

Modes generate MIDI using MIDIEventBuffer:
- output.noteOn(channel, note, velocity, delay_ms)
- output.noteOff(channel, note, delay_ms)
- output.cc(channel, controller, value, delay_ms)
```

### Step 2: Describe Your Mode

Be specific about what you want:

**Good prompt**:
```
Create Mode 6: Chord Sequencer
- Each active step plays a chord
- Pot 0: Root note (MIDI 36-96)
- Pot 1: Chord type (major/minor/dim/aug/sus/7th/9th based on value ranges)
- Pot 2: Voicing (close/open/drop2/drop3)
- Pot 3: Strum amount (0=simultaneous, 127=50ms strum)
- MIDI channel 7
- Note length: 200ms
```

**Bad prompt**:
```
Make a chord thing
```

### Step 3: Review and Refine

The LLM will generate a mode header file. Review it for:
- ✅ Correct class name (Mode6_ChordSequencer)
- ✅ Inherits from `Mode`
- ✅ Implements `processEvent()` with correct signature
- ✅ Uses `const override` on methods
- ✅ All calculations use proper types (uint8_t, unsigned long, etc.)
- ✅ MIDI note range is valid (0-127)
- ✅ Include guards (#ifndef/#define/#endif)

### Step 4: Integration

Ask the LLM to:
```
Now help me integrate Mode6_ChordSequencer:
1. Add the include to src/sequencer/Sequencer.cpp
2. Register it in Sequencer::init() on MIDI channel 7
3. Add a default pattern to DefaultSongs.cpp
```

### Step 5: Build and Test

```bash
pio run
# Check for compilation errors
# If errors, paste them back to the LLM for fixes
```

## Advanced: Multi-Step Features

For larger features, break them into steps:

### Example: Adding MIDI Input

**Prompt 1**: Architecture Planning
```
I want to add MIDI input to GRUVBOK so it can sync to external clocks.
Given the current architecture, what files would need to change?
What new classes should I create?
```

**Prompt 2**: Implementation
```
Let's implement MIDI clock input:
1. Create MIDIInput class in src/sequencer/MIDIInput.h
2. It should detect MIDI clock messages and calculate BPM
3. Integrate with Sequencer to adjust timing
Show me the code for step 1.
```

**Prompt 3**: Integration
```
Now integrate MIDIInput with the Sequencer class...
```

## Prompt Templates

### Template: New Mode

```
Create Mode [N]: [Name]

Musical Function:
[Describe what it does musically]

Parameters (4 sliders):
- Slider 0 (Pot 0): [Name] - [Range description]
- Slider 1 (Pot 1): [Name] - [Range description]
- Slider 2 (Pot 2): [Name] - [Range description]
- Slider 3 (Pot 3): [Name] - [Range description]

Behavior:
[Detailed description of how parameters affect output]

MIDI Output:
- Channel: [N]
- Note range: [Description]
- Special CCs: [If any]

Reference:
Please follow the pattern from Mode1_DrumMachine.h for simple cases,
or Mode5_BasslineProgression.h for complex pattern generation.
```

### Template: Fix/Debug

```
I'm getting this compilation error in Mode[N]:
[Paste error]

Here's the relevant code:
[Paste code section]

Context:
[Explain what you were trying to do]

Please fix the error and explain what was wrong.
```

### Template: Optimization

```
This Mode[N] is generating too many MIDI events and causing timing issues.
Current code:
[Paste processEvent() method]

Can you optimize it to:
1. Reduce MIDI message count
2. Maintain musical functionality
3. Stay under 50μs execution time
```

## Best Practices

### DO:
✅ **Start simple**: Get a basic mode working before adding complexity
✅ **Test incrementally**: Build after each change
✅ **Use existing modes as reference**: Point the LLM to similar working code
✅ **Be specific about ranges**: "0-127 maps to C1-C4" not "some note range"
✅ **Ask for explanations**: Understand what the code does
✅ **Request comments**: Well-commented code is easier to modify later

### DON'T:
❌ **Make large changes without testing**: Build and test often
❌ **Ignore warnings**: They often indicate real issues
❌ **Skip the architecture**: Understand how pieces fit together
❌ **Forget constraints**: Memory and timing matter on embedded systems
❌ **Modify core classes without understanding**: Stick to adding modes first

## Common Patterns

### Pattern: Scale-Based Note Generation

```cpp
// Ask LLM to generate using this pattern:
static constexpr uint8_t SCALE_MAJOR[7] = {0, 2, 4, 5, 7, 9, 11};

uint8_t getScaleNote(uint8_t root, int degree) {
  uint8_t octave = degree / 7;
  uint8_t note = degree % 7;
  return root + (octave * 12) + SCALE_MAJOR[note];
}
```

### Pattern: Parameter Mapping

```cpp
// Ask LLM to use this approach:
// Map slider value (0-127) to useful range
uint8_t velocity = 60 + (event.getPot(0) / 2);  // 60-123
unsigned long duration = 50 + (event.getPot(1) * 7);  // 50-939ms
uint8_t note = 36 + ((event.getPot(2) * 36) / 127);  // 36-72
```

### Pattern: Timing Subdivision

```cpp
// Generate multiple notes within a step:
static constexpr unsigned long STEP_MS = 125;  // at 120 BPM

// Eighth note subdivision:
output.noteOn(channel, note1, vel, 0);
output.noteOff(channel, note1, 60);
output.noteOn(channel, note2, vel, STEP_MS / 2);
output.noteOff(channel, note2, STEP_MS / 2 + 60);
```

## Debugging with LLMs

### Share Build Output

When you get errors:
```
Paste the entire compiler error, including file paths and line numbers.
Also include the relevant code section (with line numbers if possible).
This helps the LLM understand context.
```

### Describe Expected vs Actual

```
Mode4 should generate an ascending arpeggio but it's playing all notes simultaneously.

Expected: C, E, G, C (staggered by 125ms each)
Actual: All notes play at once

Here's my processEvent() code:
[paste code]
```

### Request Explanations

```
This code works, but I don't understand why the bit shifting is needed here:
[paste code]

Can you explain what's happening step by step?
```

## Example Session: Adding a New Mode

**User**:
```
I want to add a Mode 6 that creates rhythmic gate patterns.
Each active step generates a burst of fast notes.

Parameters:
- Pot 0: Base note (C1-C4)
- Pot 1: Number of notes in burst (1-16)
- Pot 2: Burst speed (10ms-200ms between notes)
- Pot 3: Velocity (0-127)

Can you create Mode6_GateBurst.h following the GRUVBOK pattern?
```

**LLM Response**:
[Generates complete mode file]

**User**:
```
Great! Now help me:
1. Add it to Sequencer.cpp
2. Create a demo pattern in DefaultSongs.cpp
3. Update the README to list Mode 6
```

**LLM Response**:
[Provides integration steps]

**User**:
```
I'm getting this error when I build:
error: 'GateBurst' is not a member of 'Mode6'

[pastes code]
```

**LLM Response**:
[Identifies and fixes the issue]

## Resources for LLM Context

When working with an LLM, provide links or paste these key files:

**Always include**:
- `src/modes/Mode.h` - Base class interface
- `src/core/Event.h` - Data structure
- `src/core/MIDIEvent.h` - MIDI event types

**For reference**:
- `src/modes/Mode1_DrumMachine.h` - Simple mode example
- `src/modes/Mode5_BasslineProgression.h` - Complex mode example
- `src/sequencer/MIDIScheduler.h` - Output API

## Testing Your Modes

### Manual Testing Checklist

```
Build the firmware:
☐ pio run (check for errors/warnings)

Flash to Teensy:
☐ pio run --target upload

Test in VCV Rack or DAW:
☐ Mode generates MIDI notes
☐ All 4 parameter sliders affect output
☐ Active steps play, inactive steps don't
☐ MIDI timing is correct (no hanging notes)
☐ CPU usage is acceptable (check with multiple patterns)
```

### Ask LLM to Generate Tests

```
Can you help me create a test checklist for Mode6_GateBurst?
Include:
- Parameter validation tests
- Edge cases (pot values at 0 and 127)
- Timing verification
- MIDI message correctness
```

## Advanced Topics

### Custom Data Structures

If you need mode-specific state:
```cpp
class Mode6_Sequencer : public Mode {
private:
  mutable uint8_t stepCounter[8];  // Per-track state

  // Note: Use 'mutable' for state that changes during playback
  // Modes are const during processEvent()
```

### Performance Optimization

Ask the LLM:
```
My Mode is generating 100+ MIDI events per step and causing timing issues.
Current approach: [describe]
Can you suggest a more efficient algorithm?
```

### Integration with External Hardware

```
I want Mode7 to respond to external MIDI controllers.
How can I pass MIDI input data to a mode?
Should I modify the Event structure or use a different approach?
```

## Conclusion

GRUVBOK's architecture makes it uniquely suited for LLM-assisted development. The key is:
1. **Clear communication**: Be specific about what you want
2. **Incremental progress**: Build and test often
3. **Learn the patterns**: Understand the architecture
4. **Leverage examples**: Reference existing working code

The combination of a well-structured codebase and AI assistance enables rapid prototyping of musical ideas that would take much longer to code manually.

Happy mode making! 🎵🤖
