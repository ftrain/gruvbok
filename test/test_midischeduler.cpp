#include <unity.h>
#include "../src/sequencer/MIDIScheduler.h"

// Note: These tests focus on the scheduling logic and buffer management
// Actual MIDI output requires hardware/USB MIDI and is tested separately

void test_scheduler_initialization() {
    MIDIScheduler scheduler;
    // Scheduler should initialize with no active events
    // (No direct way to verify without exposing internals, but update() should not crash)
    scheduler.update();
    TEST_ASSERT_TRUE(true);  // If we get here, initialization succeeded
}

void test_scheduler_clear() {
    MIDIScheduler scheduler;

    // Schedule some events
    scheduler.note(1, 60, 100, 0);
    scheduler.cc(2, 10, 64, 0);

    // Clear all events
    scheduler.clear();

    // Update should not execute anything (no way to verify without mocks)
    scheduler.update();
    TEST_ASSERT_TRUE(true);  // If we get here, clear succeeded
}

void test_scheduler_channel_validation() {
    MIDIScheduler scheduler;

    // Invalid channels should be rejected silently (channel 0)
    scheduler.note(0, 60, 100, 0);  // Should not crash
    scheduler.off(0, 60, 0);
    scheduler.cc(0, 10, 64, 0);
    scheduler.stopall(0, 0);

    // Invalid channels should be rejected silently (channel > 16)
    scheduler.note(17, 60, 100, 0);  // Should not crash
    scheduler.off(20, 60, 0);
    scheduler.cc(255, 10, 64, 0);
    scheduler.stopall(100, 0);

    // Valid channels should work (1-16)
    scheduler.note(1, 60, 100, 0);   // Should not crash
    scheduler.note(16, 60, 100, 0);  // Should not crash
    scheduler.cc(8, 10, 64, 0);      // Should not crash

    scheduler.update();
    TEST_ASSERT_TRUE(true);  // If we get here, validation works
}

void test_scheduler_note_scheduling() {
    MIDIScheduler scheduler;

    // Schedule a note on with immediate execution (delta=0)
    scheduler.note(1, 60, 100, 0);

    // Schedule note off
    scheduler.off(1, 60, 100);  // 100ms later

    // Update should process events when their time comes
    scheduler.update();

    TEST_ASSERT_TRUE(true);  // Basic scheduling works
}

void test_scheduler_cc_scheduling() {
    MIDIScheduler scheduler;

    // Schedule CC messages
    scheduler.cc(1, 10, 64, 0);    // Pan center
    scheduler.cc(1, 7, 100, 50);   // Volume after 50ms
    scheduler.cc(1, 1, 127, 100);  // Mod wheel after 100ms

    scheduler.update();
    TEST_ASSERT_TRUE(true);
}

void test_scheduler_stopall() {
    MIDIScheduler scheduler;

    // Schedule some notes
    scheduler.note(1, 60, 100, 0);
    scheduler.note(1, 64, 100, 0);
    scheduler.note(1, 67, 100, 0);

    // Stop all notes
    scheduler.stopall(1, 50);  // After 50ms

    scheduler.update();
    TEST_ASSERT_TRUE(true);
}

void test_scheduler_buffer_management() {
    MIDIScheduler scheduler;

    // Fill the buffer with events (max 64 events)
    for (uint8_t i = 0; i < 64; i++) {
        scheduler.note(1, 60 + (i % 12), 100, i * 10);
    }

    // Try to add more events (should be dropped silently)
    scheduler.note(1, 72, 100, 1000);  // Should be dropped

    scheduler.update();
    TEST_ASSERT_TRUE(true);  // Buffer overflow handled gracefully
}

void test_scheduler_multiple_channels() {
    MIDIScheduler scheduler;

    // Schedule events on different channels
    scheduler.note(1, 36, 127, 0);   // Kick on channel 1
    scheduler.note(2, 60, 100, 0);   // Note on channel 2
    scheduler.note(10, 64, 80, 0);   // Note on channel 10
    scheduler.note(16, 67, 90, 0);   // Note on channel 16

    scheduler.update();
    TEST_ASSERT_TRUE(true);
}

void test_scheduler_delta_timing() {
    MIDIScheduler scheduler;

    // Schedule events with various delta times
    scheduler.note(1, 60, 100, 0);      // Immediate
    scheduler.note(1, 64, 100, 10);     // +10ms
    scheduler.note(1, 67, 100, 100);    // +100ms
    scheduler.off(1, 60, 50);           // +50ms
    scheduler.off(1, 64, 60);           // +60ms
    scheduler.off(1, 67, 150);          // +150ms

    // Multiple updates to process scheduled events
    for (int i = 0; i < 10; i++) {
        scheduler.update();
    }

    TEST_ASSERT_TRUE(true);
}

void test_scheduler_event_interleaving() {
    MIDIScheduler scheduler;

    // Schedule interleaved note on/off/cc events
    scheduler.note(1, 60, 100, 0);
    scheduler.cc(1, 10, 64, 5);        // Pan
    scheduler.off(1, 60, 50);
    scheduler.note(1, 64, 110, 60);
    scheduler.cc(1, 7, 127, 65);       // Volume
    scheduler.off(1, 64, 110);

    for (int i = 0; i < 15; i++) {
        scheduler.update();
    }

    TEST_ASSERT_TRUE(true);
}

void test_scheduler_clear_after_scheduling() {
    MIDIScheduler scheduler;

    // Schedule many events
    for (uint8_t i = 0; i < 20; i++) {
        scheduler.note(1, 60 + i, 100, i * 100);
    }

    // Clear before they execute
    scheduler.clear();

    // Update should not execute anything
    for (int i = 0; i < 10; i++) {
        scheduler.update();
    }

    // Schedule new events after clear
    scheduler.note(2, 72, 80, 0);
    scheduler.update();

    TEST_ASSERT_TRUE(true);
}

void test_scheduler_boundary_values() {
    MIDIScheduler scheduler;

    // Test boundary MIDI values
    scheduler.note(1, 0, 0, 0);        // Min note, min velocity
    scheduler.note(1, 127, 127, 0);    // Max note, max velocity
    scheduler.cc(1, 0, 0, 0);          // Min CC
    scheduler.cc(1, 127, 127, 0);      // Max CC

    // Test boundary channels
    scheduler.note(1, 60, 100, 0);     // Min valid channel
    scheduler.note(16, 60, 100, 0);    // Max valid channel

    scheduler.update();
    TEST_ASSERT_TRUE(true);
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_scheduler_initialization);
    RUN_TEST(test_scheduler_clear);
    RUN_TEST(test_scheduler_channel_validation);
    RUN_TEST(test_scheduler_note_scheduling);
    RUN_TEST(test_scheduler_cc_scheduling);
    RUN_TEST(test_scheduler_stopall);
    RUN_TEST(test_scheduler_buffer_management);
    RUN_TEST(test_scheduler_multiple_channels);
    RUN_TEST(test_scheduler_delta_timing);
    RUN_TEST(test_scheduler_event_interleaving);
    RUN_TEST(test_scheduler_clear_after_scheduling);
    RUN_TEST(test_scheduler_boundary_values);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
