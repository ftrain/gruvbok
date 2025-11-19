# Contributing to GRUVBOK

Thank you for your interest in contributing to GRUVBOK! This document provides guidelines and instructions for contributing.

## Ways to Contribute

- **Add New Modes**: Create new musical modes (synthesizers, sequencers, effects)
- **Improve Documentation**: Fix typos, add examples, clarify explanations
- **Fix Bugs**: Report and fix issues
- **Optimize Performance**: Improve timing, reduce memory usage
- **Add Features**: New hardware support, MIDI features, UI improvements
- **Share Patterns**: Contribute demo songs and patterns

## Getting Started

1. **Fork the Repository**
   ```bash
   git clone https://github.com/yourusername/gruvbok.git
   cd gruvbok
   ```

2. **Set Up Development Environment**
   ```bash
   # Install PlatformIO
   pip install platformio

   # Build to verify setup
   pio run
   ```

3. **Create a Branch**
   ```bash
   git checkout -b feature/my-new-mode
   # or
   git checkout -b fix/timing-issue
   ```

## Code Style

### C++ Guidelines

- **Naming**:
  - Classes: `PascalCase` (e.g., `DrumMachine`)
  - Functions/methods: `camelCase` (e.g., `processEvent`)
  - Variables: `camelCase` (e.g., `noteVelocity`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_VELOCITY`)
  - Private members: prefix with `m_` if needed for clarity

- **Formatting**:
  - Indentation: 2 spaces (no tabs)
  - Braces: Same line for functions, next line for classes
  - Line length: 100 characters max
  - Comments: `//` for single line, `/* */` for multi-line

- **Modern C++**:
  - Use `constexpr` for compile-time constants
  - Use `const` and `const override` appropriately
  - Prefer `uint8_t`, `uint16_t` over `unsigned char`, `unsigned int`
  - Use `nullptr` instead of `NULL`

### Example Mode Structure

```cpp
#ifndef MODE6_YOURMODE_H
#define MODE6_YOURMODE_H

#include "Mode.h"

/**
 * Mode6 - Your Mode Name
 *
 * Brief description of what this mode does.
 *
 * Event interpretation:
 * - Switch: What happens when active
 * - Slider 0: Parameter description
 * - Slider 1: Parameter description
 * - Slider 2: Parameter description
 * - Slider 3: Parameter description
 */
class Mode6_YourMode : public Mode {
private:
  static constexpr uint8_t MIN_NOTE = 36;
  static constexpr uint8_t MAX_NOTE = 96;

public:
  Mode6_YourMode(uint8_t channel) : Mode(channel) {}

  void processEvent(uint8_t trackIndex, const Event& event,
                   unsigned long stepTime, MIDIEventBuffer& output) const override {
    if (!event.getSwitch()) return;

    // Implementation here
  }

  const char* getName() const override {
    return "YourMode";
  }
};

#endif // MODE6_YOURMODE_H
```

## Adding a New Mode

### Step-by-Step Process

1. **Create Mode File**
   - File: `src/modes/ModeN_YourMode.h`
   - Follow naming convention: `Mode6_ChordSequencer.h`

2. **Implement Required Methods**
   ```cpp
   // Constructor
   ModeN_YourMode(uint8_t channel) : Mode(channel) {}

   // Process event (main logic)
   void processEvent(uint8_t trackIndex, const Event& event,
                    unsigned long stepTime, MIDIEventBuffer& output) const override

   // Name for debugging
   const char* getName() const override
   ```

3. **Register Mode**

   Edit `src/sequencer/Sequencer.cpp`:
   ```cpp
   // Add include
   #include "../modes/ModeN_YourMode.h"

   // Register in init()
   modes[N] = new ModeN_YourMode(MIDI_CHANNEL);
   ```

4. **Add Demo Pattern** (optional but recommended)

   Edit `src/core/DefaultSongs.cpp`:
   ```cpp
   // Add example pattern
   Pattern& yourMode = song.getPattern(N, 0);
   setEvent(yourMode, 0, 0, true, 64, 64, 64, 64);
   ```

5. **Update Documentation**
   - Add mode description to README.md
   - Document parameters and behavior
   - Include usage examples

## Testing Requirements

### Before Submitting

- [ ] Code compiles without errors: `pio run`
- [ ] No compiler warnings
- [ ] Mode appears in mode list (check getName())
- [ ] All 4 parameters work as documented
- [ ] Active steps generate MIDI, inactive don't
- [ ] No hanging notes (all note-ons have note-offs)
- [ ] Memory usage is reasonable (check build output)
- [ ] Timing is correct (no jitter or delays)

### Manual Testing Checklist

- [ ] Test with all parameter sliders at minimum (0)
- [ ] Test with all parameter sliders at maximum (127)
- [ ] Test with all parameter sliders at midpoint (64)
- [ ] Test with multiple tracks active simultaneously
- [ ] Test pattern switching (values persist correctly)
- [ ] Test in VCV Rack or DAW (MIDI output is correct)

## Pull Request Process

1. **Ensure Your Branch is Up to Date**
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Commit Your Changes**
   ```bash
   git add src/modes/Mode6_YourMode.h
   git commit -m "Add Mode 6: Chord Sequencer

   - Implements major/minor/diminished chord types
   - Supports 4 voicing options
   - Adds strum effect with configurable timing
   - Includes demo pattern in DefaultSongs"
   ```

   **Good commit message**:
   - First line: Brief summary (50 chars or less)
   - Blank line
   - Detailed description (what, why, how)
   - Reference issues if applicable (#123)

3. **Push to Your Fork**
   ```bash
   git push origin feature/my-new-mode
   ```

4. **Create Pull Request**
   - Go to GitHub and create PR
   - Fill out the PR template
   - Link any related issues
   - Add screenshots/videos if applicable

### PR Template

```markdown
## Description
Brief description of what this PR does.

## Type of Change
- [ ] New mode
- [ ] Bug fix
- [ ] Feature enhancement
- [ ] Documentation
- [ ] Performance optimization

## Testing
- [ ] Compiles without errors
- [ ] Tested on hardware (if applicable)
- [ ] Tested in VCV Rack/DAW
- [ ] No memory issues
- [ ] All parameters work correctly

## Checklist
- [ ] Code follows style guidelines
- [ ] Comments added for complex logic
- [ ] Documentation updated
- [ ] Demo pattern included (for new modes)

## Additional Notes
Any additional context, screenshots, or demo videos.
```

## Reporting Bugs

### Bug Report Template

```markdown
**Describe the bug**
Clear description of what's wrong.

**To Reproduce**
Steps to reproduce:
1. Set Mode to X
2. Set Pattern to Y
3. Press button Z
4. See error

**Expected behavior**
What should happen.

**Actual behavior**
What actually happens.

**Hardware**
- Teensy version: [e.g., 4.1]
- Firmware version: [git commit hash]
- Connected hardware: [buttons, pots, etc.]

**MIDI Setup**
- DAW/Software: [e.g., VCV Rack 2.0]
- Operating System: [e.g., macOS 14.0]

**Additional context**
Any other relevant information.
```

## Feature Requests

We love new ideas! Please check if a similar feature has already been requested before creating a new issue.

**Feature Request Template**:
```markdown
**Is your feature request related to a problem?**
Clear description of the problem.

**Describe the solution you'd like**
What you want to happen.

**Describe alternatives you've considered**
Other approaches you've thought about.

**Additional context**
Mock-ups, examples, or references.
```

## Documentation Contributions

Documentation improvements are always welcome:

- Fix typos and grammar
- Add code examples
- Improve explanations
- Add diagrams or illustrations
- Translate documentation

Edit files in `/docs` directory and submit a PR.

## Community Guidelines

- **Be respectful**: Treat everyone with kindness
- **Be constructive**: Provide helpful feedback
- **Be patient**: Maintainers are volunteers
- **Be collaborative**: Work together to solve problems

## Questions?

- **Documentation**: Check `/docs` directory
- **Issues**: Search existing issues first
- **Discussions**: Use GitHub Discussions for general questions
- **AI Development**: See `docs/AI_DEVELOPMENT.md` for LLM assistance

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to GRUVBOK! 🎹✨
