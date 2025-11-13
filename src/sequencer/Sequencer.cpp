#include "Sequencer.h"
#include "../modes/Mode0_PatternSequencer.h"
#include "../modes/Mode1_DrumMachine.h"
#include "../modes/Mode2_AcidBass.h"
#include "../core/MIDIEvent.h"
#include <Arduino.h>

Sequencer::Sequencer(Song* s, Hardware* hw, MIDIScheduler* sched)
  : song(s), hardware(hw), scheduler(sched),
    currentStep(0), currentTrack(0), currentMode(1),  // Mode 1 for drum machine
    sequencePosition(0),  // Start at beginning of Mode 0 sequence
    bpm(120.0), sendClock(true), isPlaying(false) {

  // Initialize all modes to nullptr
  for (uint8_t i = 0; i < 15; i++) {
    modes[i] = nullptr;
    currentPatterns[i] = 0;
  }
}

Sequencer::~Sequencer() {
  // Clean up mode instances
  for (uint8_t i = 0; i < 15; i++) {
    if (modes[i] != nullptr) {
      delete modes[i];
    }
  }
}

void Sequencer::init() {
  // Create mode instances
  // Mode 0: Pattern Sequencer (channel 1)
  modes[0] = new Mode0_PatternSequencer(1);

  // Mode 1: Drum Machine (channel 2)
  modes[1] = new Mode1_DrumMachine(2);

  // Mode 2: Acid Bass (channel 3)
  modes[2] = new Mode2_AcidBass(3);

  // Modes 3-14: Not yet implemented, set to nullptr
  // You can add more modes here as they're implemented

  // Modes no longer need scheduler reference - they're pure functions!
  // They return MIDIEvents which we schedule in bulk

  // Calculate timing intervals
  calculateIntervals();

  // Initialize timing
  lastStepTime = millis();
  lastClockTime = millis();
}

void Sequencer::start() {
  isPlaying = true;
  currentStep = 0;
  lastStepTime = millis();
  lastClockTime = millis();

  // Send MIDI Start message
  usbMIDI.sendRealTime(usbMIDI.Start);
}

void Sequencer::stop() {
  isPlaying = false;

  // Send MIDI Stop message
  usbMIDI.sendRealTime(usbMIDI.Stop);

  // Stop all notes on all channels
  for (uint8_t i = 0; i < 15; i++) {
    if (modes[i] != nullptr) {
      scheduler->stopall(modes[i]->getChannel(), 0);
    }
  }
}

void Sequencer::setBPM(float newBPM) {
  // Clamp BPM to range
  if (newBPM < 20.0) newBPM = 20.0;
  if (newBPM > 800.0) newBPM = 800.0;

  bpm = newBPM;
  calculateIntervals();
}

void Sequencer::update() {
  unsigned long currentTime = millis();

  // Handle user input
  handleInput();

  // Send MIDI clock pulses if enabled
  if (isPlaying && sendClock) {
    if (currentTime - lastClockTime >= clockInterval) {
      sendClockPulse();
    }
  }

  // Advance sequencer steps
  if (isPlaying) {
    while (currentTime - lastStepTime >= stepInterval) {
      lastStepTime += stepInterval;  // Accumulate time to prevent drift
      advanceStep();
      processStep();
    }
  }

  // Update MIDI scheduler (execute scheduled events)
  scheduler->update();

  // Keep USB MIDI running
  while (usbMIDI.read()) {
    // Process incoming MIDI if needed
  }
}

void Sequencer::advanceStep() {
  // Move to next step
  currentStep = (currentStep + 1) % 16;

  // When pattern completes (step 0), check Mode 0 sequence
  if (currentStep == 0) {
    updatePatternFromSequence();
    hardware->setLEDBrightness(255);  // Full brightness on step 0
  } else {
    hardware->setLEDBrightness(5);    // Very dim on all other steps
  }
}

void Sequencer::processStep() {
  unsigned long stepTime = millis();

  // Create event buffer for collecting MIDI events from all modes
  MIDIEventBuffer eventBuffer;

  // PURE FUNCTIONAL DESIGN:
  // 1. Collect events from all modes (pure functions, no side effects)
  // 2. Schedule all events in bulk (single point of I/O)

  // Process all active modes
  for (uint8_t modeIndex = 0; modeIndex < 15; modeIndex++) {
    if (modes[modeIndex] == nullptr) continue;

    // Get current pattern for this mode
    uint8_t patternIndex = currentPatterns[modeIndex];
    Pattern& pattern = song->getPattern(modeIndex, patternIndex);

    // Process all tracks in this pattern
    for (uint8_t trackIndex = 0; trackIndex < Pattern::getNumTracks(); trackIndex++) {
      Track& track = pattern.getTrack(trackIndex);
      const Event& event = track.getEvent(currentStep);

      // Let the mode generate MIDI events (pure function!)
      modes[modeIndex]->processEvent(trackIndex, event, stepTime, eventBuffer);

      // If buffer is getting full, schedule events now and clear
      if (eventBuffer.remaining() < 8) {
        scheduler->scheduleAll(eventBuffer);
        eventBuffer.clear();
      }
    }
  }

  // Schedule any remaining events
  if (!eventBuffer.isEmpty()) {
    scheduler->scheduleAll(eventBuffer);
  }
}

void Sequencer::sendClockPulse() {
  lastClockTime = millis();
  usbMIDI.sendRealTime(usbMIDI.Clock);
}

void Sequencer::calculateIntervals() {
  // Step interval: 16th notes at current BPM
  // (60 seconds / BPM) * 1000 ms * (1 beat / 4 steps) = ms per step
  stepInterval = (unsigned long)((60000.0 / bpm) / 4.0);

  // MIDI clock: 24 PPQN (pulses per quarter note)
  clockInterval = (unsigned long)((60000.0 / bpm) / 24.0);
}

void Sequencer::handleInput() {
  // ========================================
  // BUTTONS: Toggle events via mode's recordEvent()
  // ========================================
  for (uint8_t i = 0; i < 16; i++) {
    if (hardware->readButtonPress(i)) {
      recordEvent(i, true);
    }
  }

  // ========================================
  // POTS: Navigation (mode-agnostic)
  // ========================================

  // Pot 0: Tempo (20-800 BPM, centered at 120)
  int16_t tempoPot = hardware->readPotChange(0, 2);
  if (tempoPot >= 0) {
    float normalized = tempoPot / 127.0;
    float newBPM = (normalized < 0.5)
      ? 20.0 + (normalized * 2.0) * 100.0                    // Linear 20-120
      : 120.0 + pow((normalized - 0.5) * 2.0, 2.0) * 680.0;  // Quadratic 120-800
    setBPM(newBPM);
  }

  // Pot 1: Mode selection (0-14)
  int16_t modePot = hardware->readPotChange(1, 2);
  if (modePot >= 0) {
    uint8_t newMode = (modePot * 15) / 128;  // Use 128 to prevent overflow to 15
    if (newMode > 14) newMode = 14;
    setCurrentMode(newMode);
    usbMIDI.sendControlChange(1, newMode, 16);  // Debug CC
  }

  // Pot 2: Pattern selection (0-31)
  int16_t patternPot = hardware->readPotChange(2, 2);
  if (patternPot >= 0) {
    uint8_t newPattern = (patternPot * 32) / 128;  // Use 128 to prevent overflow to 32
    if (newPattern > 31) newPattern = 31;
    currentPatterns[currentMode] = newPattern;
    usbMIDI.sendControlChange(2, newPattern, 16);  // Debug CC
  }

  // Pot 3: Track selection (0-7)
  int16_t trackPot = hardware->readPotChange(3, 2);
  if (trackPot >= 0) {
    uint8_t newTrack = (trackPot * 8) / 128;  // Use 128 to prevent overflow to 8
    if (newTrack > 7) newTrack = 7;
    setCurrentTrack(newTrack);
    usbMIDI.sendControlChange(3, newTrack, 16);  // Debug CC
  }

  // ========================================
  // SLIDERS: Debug CCs (always send, no threshold)
  // ========================================
  static unsigned long lastSliderDebug = 0;
  if (millis() - lastSliderDebug > 50) {  // Send every 50ms
    lastSliderDebug = millis();
    for (uint8_t i = 0; i < 4; i++) {
      uint8_t sliderValue = hardware->readSlider(i);
      // CC20-23 on channel 2 (drum machine channel)
      usbMIDI.sendControlChange(20 + i, sliderValue, 2);
    }
  }
}

void Sequencer::recordEvent(uint8_t buttonIndex, bool state) {
  // Button index maps directly to step index
  uint8_t stepIndex = buttonIndex;

  // Get current pattern and track
  Pattern& pattern = song->getPattern(currentMode, currentPatterns[currentMode]);
  Track& track = pattern.getTrack(currentTrack);
  Event& event = track.getEvent(stepIndex);

  // Toggle the switch
  event.toggleSwitch();

  // Capture hardware state directly into event pots
  // Recording is mode-agnostic: just store the raw slider values
  // The mode will interpret these values during playback
  InputState inputs = hardware->getCurrentState();
  for (uint8_t i = 0; i < 4; i++) {
    event.setPot(i, inputs.sliders[i]);
  }
}

void Sequencer::updatePatternFromSequence() {
  // Mode 0 controls pattern sequencing
  // Read Mode 0, Pattern 0, Track 0 to get the sequence
  Pattern& mode0Pattern = song->getPattern(0, 0);
  Track& sequenceTrack = mode0Pattern.getTrack(0);

  // Read current sequence position
  const Event& sequenceEvent = sequenceTrack.getEvent(sequencePosition);

  // If this slot has a pattern programmed (switch is on)
  if (sequenceEvent.getSwitch()) {
    // Get pattern number from pot 0 (0-127 maps to 0-31)
    uint8_t patternNumber = (sequenceEvent.getPot(0) * 32) / 128;
    if (patternNumber > 31) patternNumber = 31;

    // Update all modes (except Mode 0) to use this pattern
    for (uint8_t i = 1; i < 15; i++) {
      currentPatterns[i] = patternNumber;
    }

    // Advance to next sequence position
    sequencePosition++;
    if (sequencePosition >= 16) {
      sequencePosition = 0;  // Loop back to start
    }
  } else {
    // Empty slot - loop back to beginning
    sequencePosition = 0;

    // Re-read the first slot
    const Event& firstEvent = sequenceTrack.getEvent(0);
    if (firstEvent.getSwitch()) {
      uint8_t patternNumber = (firstEvent.getPot(0) * 32) / 128;
      if (patternNumber > 31) patternNumber = 31;
      for (uint8_t i = 1; i < 15; i++) {
        currentPatterns[i] = patternNumber;
      }
    }
  }
}
