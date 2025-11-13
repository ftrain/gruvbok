#include <unity.h>
#include "../src/modes/Mode1_DrumMachine.h"
#include "../src/core/MIDIEvent.h"

// Test that modes are truly pure functions with no side effects

void test_mode_pure_functional_deterministic() {
    Mode1_DrumMachine mode(2);  // Channel 2

    // Create an event with specific parameters
    Event event(true, 100, 20, 50, 64);  // Switch on, velocity=100, flam=20, length=50, pan=64

    // Process event twice with same inputs
    MIDIEventBuffer buffer1;
    mode.processEvent(0, event, 1000, buffer1);

    MIDIEventBuffer buffer2;
    mode.processEvent(0, event, 1000, buffer2);

    // Should produce identical results (deterministic)
    TEST_ASSERT_EQUAL(buffer1.size(), buffer2.size());

    // Verify all events match
    for (uint8_t i = 0; i < buffer1.size(); i++) {
        TEST_ASSERT_EQUAL(buffer1[i].type, buffer2[i].type);
        TEST_ASSERT_EQUAL(buffer1[i].channel, buffer2[i].channel);
        TEST_ASSERT_EQUAL(buffer1[i].data1, buffer2[i].data1);
        TEST_ASSERT_EQUAL(buffer1[i].data2, buffer2[i].data2);
        TEST_ASSERT_EQUAL(buffer1[i].delta, buffer2[i].delta);
    }
}

void test_mode_produces_events_not_side_effects() {
    Mode1_DrumMachine mode(2);

    // Create event: kick drum with velocity
    Event event(true, 127, 0, 64, 64);  // max velocity, no flam, medium length, center pan

    // Process event
    MIDIEventBuffer buffer;
    mode.processEvent(0, event, 0, buffer);

    // Should produce events in buffer (not schedule directly)
    TEST_ASSERT_GREATER_THAN(0, buffer.size());

    // Should have note on and note off
    bool hasNoteOn = false;
    bool hasNoteOff = false;

    for (uint8_t i = 0; i < buffer.size(); i++) {
        if (buffer[i].type == MIDIEvent::NOTE_ON) hasNoteOn = true;
        if (buffer[i].type == MIDIEvent::NOTE_OFF) hasNoteOff = true;
    }

    TEST_ASSERT_TRUE(hasNoteOn);
    TEST_ASSERT_TRUE(hasNoteOff);
}

void test_mode_switch_off_produces_no_events() {
    Mode1_DrumMachine mode(2);

    // Create event with switch OFF
    Event event(false, 100, 0, 64, 64);

    // Process event
    MIDIEventBuffer buffer;
    mode.processEvent(0, event, 0, buffer);

    // Should produce no events
    TEST_ASSERT_EQUAL(0, buffer.size());
}

void test_mode_flam_produces_multiple_notes() {
    Mode1_DrumMachine mode(2);

    // Create event with flam
    Event event(true, 100, 64, 50, 0);  // Velocity=100, flam=64, length=50, no pan

    // Process event
    MIDIEventBuffer buffer;
    mode.processEvent(0, event, 0, buffer);

    // Should produce 4 events: flam note on/off + main note on/off
    TEST_ASSERT_EQUAL(4, buffer.size());

    // Count note ons and offs
    uint8_t noteOns = 0;
    uint8_t noteOffs = 0;

    for (uint8_t i = 0; i < buffer.size(); i++) {
        if (buffer[i].type == MIDIEvent::NOTE_ON) noteOns++;
        if (buffer[i].type == MIDIEvent::NOTE_OFF) noteOffs++;
    }

    TEST_ASSERT_EQUAL(2, noteOns);   // Flam + main
    TEST_ASSERT_EQUAL(2, noteOffs);  // Flam + main
}

void test_mode_pan_produces_cc_event() {
    Mode1_DrumMachine mode(2);

    // Create event with pan set
    Event event(true, 100, 0, 50, 80);  // Velocity=100, no flam, length=50, pan=80

    // Process event
    MIDIEventBuffer buffer;
    mode.processEvent(0, event, 0, buffer);

    // Should produce note on, note off, and CC for pan
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(3, buffer.size());

    // Find CC event
    bool hasPanCC = false;
    for (uint8_t i = 0; i < buffer.size(); i++) {
        if (buffer[i].type == MIDIEvent::CC && buffer[i].data1 == 10) {
            hasPanCC = true;
            TEST_ASSERT_EQUAL(80, buffer[i].data2);  // Pan value
        }
    }

    TEST_ASSERT_TRUE(hasPanCC);
}

void test_mode_correct_channel() {
    Mode1_DrumMachine mode(5);  // Channel 5

    Event event(true, 100, 0, 50, 0);

    MIDIEventBuffer buffer;
    mode.processEvent(0, event, 0, buffer);

    // All events should be on channel 5
    for (uint8_t i = 0; i < buffer.size(); i++) {
        TEST_ASSERT_EQUAL(5, buffer[i].channel);
    }
}

void test_mode_correct_drum_notes() {
    Mode1_DrumMachine mode(10);  // GM drums typically on channel 10

    Event kickEvent(true, 100, 0, 50, 0);
    Event snareEvent(true, 100, 0, 50, 0);

    // Track 0 = Kick (note 36)
    MIDIEventBuffer kickBuffer;
    mode.processEvent(0, kickEvent, 0, kickBuffer);
    bool hasKick = false;
    for (uint8_t i = 0; i < kickBuffer.size(); i++) {
        if (kickBuffer[i].type == MIDIEvent::NOTE_ON && kickBuffer[i].data1 == 36) {
            hasKick = true;
        }
    }
    TEST_ASSERT_TRUE(hasKick);

    // Track 1 = Snare (note 38)
    MIDIEventBuffer snareBuffer;
    mode.processEvent(1, snareEvent, 0, snareBuffer);
    bool hasSnare = false;
    for (uint8_t i = 0; i < snareBuffer.size(); i++) {
        if (snareBuffer[i].type == MIDIEvent::NOTE_ON && snareBuffer[i].data1 == 38) {
            hasSnare = false;
        }
    }
    TEST_ASSERT_TRUE(hasSnare);
}

void test_mode_buffer_isolation() {
    Mode1_DrumMachine mode(2);

    Event event1(true, 100, 0, 50, 0);
    Event event2(true, 110, 0, 60, 0);

    // Process into separate buffers
    MIDIEventBuffer buffer1;
    mode.processEvent(0, event1, 0, buffer1);

    MIDIEventBuffer buffer2;
    mode.processEvent(0, event2, 0, buffer2);

    // Buffers should be independent
    TEST_ASSERT_NOT_EQUAL(0, buffer1.size());
    TEST_ASSERT_NOT_EQUAL(0, buffer2.size());

    // Different velocities should produce different events
    bool foundDifferentVelocity = false;
    for (uint8_t i = 0; i < buffer1.size() && i < buffer2.size(); i++) {
        if (buffer1[i].type == MIDIEvent::NOTE_ON && buffer2[i].type == MIDIEvent::NOTE_ON) {
            if (buffer1[i].data2 != buffer2[i].data2) {
                foundDifferentVelocity = true;
            }
        }
    }
    TEST_ASSERT_TRUE(foundDifferentVelocity);
}

void test_midieventbuffer_operations() {
    MIDIEventBuffer buffer;

    TEST_ASSERT_TRUE(buffer.isEmpty());
    TEST_ASSERT_EQUAL(0, buffer.size());

    // Add note on
    bool added = buffer.noteOn(1, 60, 100, 0);
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_EQUAL(1, buffer.size());
    TEST_ASSERT_FALSE(buffer.isEmpty());

    // Add note off
    buffer.noteOff(1, 60, 100);
    TEST_ASSERT_EQUAL(2, buffer.size());

    // Add CC
    buffer.cc(1, 10, 64, 0);
    TEST_ASSERT_EQUAL(3, buffer.size());

    // Clear
    buffer.clear();
    TEST_ASSERT_TRUE(buffer.isEmpty());
    TEST_ASSERT_EQUAL(0, buffer.size());
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_mode_pure_functional_deterministic);
    RUN_TEST(test_mode_produces_events_not_side_effects);
    RUN_TEST(test_mode_switch_off_produces_no_events);
    RUN_TEST(test_mode_flam_produces_multiple_notes);
    RUN_TEST(test_mode_pan_produces_cc_event);
    RUN_TEST(test_mode_correct_channel);
    RUN_TEST(test_mode_correct_drum_notes);
    RUN_TEST(test_mode_buffer_isolation);
    RUN_TEST(test_midieventbuffer_operations);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
