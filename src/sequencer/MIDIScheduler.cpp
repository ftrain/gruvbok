#include "MIDIScheduler.h"
#include <Arduino.h>

void MIDIScheduler::note(uint8_t channel, uint8_t pitch, uint8_t velocity, unsigned long delta) {
  // Validate MIDI channel (1-16)
  if (channel == 0 || channel > 16) return;
  // pitch and velocity are already 0-127 due to uint8_t range
  scheduleEvent(ScheduledEvent::NOTE_ON, channel, pitch, velocity, delta);
}

void MIDIScheduler::off(uint8_t channel, uint8_t pitch, unsigned long delta) {
  // Validate MIDI channel (1-16)
  if (channel == 0 || channel > 16) return;
  scheduleEvent(ScheduledEvent::NOTE_OFF, channel, pitch, 0, delta);
}

void MIDIScheduler::cc(uint8_t channel, uint8_t controller, uint8_t value, unsigned long delta) {
  // Validate MIDI channel (1-16)
  if (channel == 0 || channel > 16) return;
  scheduleEvent(ScheduledEvent::CC, channel, controller, value, delta);
}

void MIDIScheduler::stopall(uint8_t channel, unsigned long delta) {
  // Validate MIDI channel (1-16)
  if (channel == 0 || channel > 16) return;
  scheduleEvent(ScheduledEvent::STOP_ALL, channel, 0, 0, delta);
}

uint8_t MIDIScheduler::scheduleAll(const MIDIEventBuffer& buffer) {
  uint8_t scheduled = 0;

  // Iterate through all events in buffer and schedule them
  for (uint8_t i = 0; i < buffer.size(); i++) {
    const MIDIEvent& event = buffer[i];

    // Convert MIDIEvent::Type to ScheduledEvent::Type
    ScheduledEvent::Type type;
    switch (event.type) {
      case MIDIEvent::NOTE_ON:
        type = ScheduledEvent::NOTE_ON;
        break;
      case MIDIEvent::NOTE_OFF:
        type = ScheduledEvent::NOTE_OFF;
        break;
      case MIDIEvent::CC:
        type = ScheduledEvent::CC;
        break;
      case MIDIEvent::STOP_ALL:
        type = ScheduledEvent::STOP_ALL;
        break;
      default:
        continue;  // Skip unknown types
    }

    // Validate channel
    if (event.channel == 0 || event.channel > 16) {
      continue;  // Skip invalid channels
    }

    // Schedule the event
    int8_t slot = findFreeSlot();
    if (slot < 0) {
      // Buffer full, can't schedule more events
      break;
    }

    events[slot].type = type;
    events[slot].channel = event.channel;
    events[slot].data1 = event.data1;
    events[slot].data2 = event.data2;
    events[slot].executeTime = millis() + event.delta;
    events[slot].active = true;

    scheduled++;
  }

  return scheduled;
}

void MIDIScheduler::update() {
  unsigned long currentTime = millis();

  for (uint8_t i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    if (events[i].active && currentTime >= events[i].executeTime) {
      // Execute the event
      switch (events[i].type) {
        case ScheduledEvent::NOTE_ON:
          usbMIDI.sendNoteOn(events[i].data1, events[i].data2, events[i].channel);
          break;

        case ScheduledEvent::NOTE_OFF:
          usbMIDI.sendNoteOff(events[i].data1, 0, events[i].channel);
          break;

        case ScheduledEvent::CC:
          usbMIDI.sendControlChange(events[i].data1, events[i].data2, events[i].channel);
          break;

        case ScheduledEvent::STOP_ALL:
          // Send all notes off CC (123)
          usbMIDI.sendControlChange(123, 0, events[i].channel);
          break;
      }

      // Mark slot as free
      events[i].active = false;
    }
  }
}

void MIDIScheduler::clear() {
  for (uint8_t i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    events[i].active = false;
  }
}

int8_t MIDIScheduler::findFreeSlot() {
  for (uint8_t i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    if (!events[i].active) {
      return i;
    }
  }
  return -1;  // No free slots
}

void MIDIScheduler::scheduleEvent(ScheduledEvent::Type type, uint8_t channel,
                                  uint8_t data1, uint8_t data2, unsigned long delta) {
  int8_t slot = findFreeSlot();
  if (slot < 0) {
    return;  // Buffer full, drop event
  }

  events[slot].type = type;
  events[slot].channel = channel;
  events[slot].data1 = data1;
  events[slot].data2 = data2;
  events[slot].executeTime = millis() + delta;
  events[slot].active = true;
}
