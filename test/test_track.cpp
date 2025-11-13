#include <unity.h>
#include "../src/core/Track.h"

void test_track_initialization() {
    Track track;

    // All events should be empty
    for (uint8_t i = 0; i < 16; i++) {
        TEST_ASSERT_TRUE(track[i].isEmpty());
    }
}

void test_track_event_access() {
    Track track;

    // Set event at index 5
    track[5].setSwitch(true);
    track[5].setPot(0, 100);

    TEST_ASSERT_TRUE(track[5].getSwitch());
    TEST_ASSERT_EQUAL(100, track[5].getPot(0));

    // Other events should be unaffected
    TEST_ASSERT_FALSE(track[0].getSwitch());
    TEST_ASSERT_FALSE(track[15].getSwitch());
}

void test_track_has_active_events() {
    Track track;

    TEST_ASSERT_FALSE(track.hasActiveEvents());

    track[3].setSwitch(true);
    TEST_ASSERT_TRUE(track.hasActiveEvents());

    track[7].setSwitch(true);
    TEST_ASSERT_TRUE(track.hasActiveEvents());

    track.clear();
    TEST_ASSERT_FALSE(track.hasActiveEvents());
}

void test_track_count_active_events() {
    Track track;

    TEST_ASSERT_EQUAL(0, track.countActiveEvents());

    track[0].setSwitch(true);
    track[4].setSwitch(true);
    track[8].setSwitch(true);
    track[12].setSwitch(true);

    TEST_ASSERT_EQUAL(4, track.countActiveEvents());
}

void test_track_clear() {
    Track track;

    // Set some events
    track[0].setSwitch(true);
    track[5].setPot(0, 64);
    track[10].setSwitch(true);

    TEST_ASSERT_TRUE(track.hasActiveEvents());

    // Clear track
    track.clear();

    // All events should be empty
    TEST_ASSERT_FALSE(track.hasActiveEvents());
    TEST_ASSERT_EQUAL(0, track[5].getPot(0));
}

void test_track_index_wrapping() {
    Track track;

    // Set event at index 5
    track[5].setSwitch(true);

    // Access with wrapped index (5 + 16 = 21, should wrap to 5)
    TEST_ASSERT_TRUE(track[21].getSwitch());

    // Access with mask (should access same event)
    TEST_ASSERT_TRUE(track[5 & 0x0F].getSwitch());
}

void test_track_memory_size() {
    // Track should be 16 events × 4 bytes = 64 bytes
    TEST_ASSERT_EQUAL(64, sizeof(Track));
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_track_initialization);
    RUN_TEST(test_track_event_access);
    RUN_TEST(test_track_has_active_events);
    RUN_TEST(test_track_count_active_events);
    RUN_TEST(test_track_clear);
    RUN_TEST(test_track_index_wrapping);
    RUN_TEST(test_track_memory_size);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
