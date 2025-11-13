#include <unity.h>
#include "../src/core/Pattern.h"

void test_pattern_initialization() {
    Pattern pattern;

    // All tracks should be initialized
    for (uint8_t t = 0; t < 8; t++) {
        for (uint8_t e = 0; e < 16; e++) {
            TEST_ASSERT_TRUE(pattern[t][e].isEmpty());
        }
    }
}

void test_pattern_track_access() {
    Pattern pattern;

    // Set event on track 3, step 7
    pattern[3][7].setSwitch(true);
    pattern[3][7].setPot(0, 64);

    TEST_ASSERT_TRUE(pattern[3][7].getSwitch());
    TEST_ASSERT_EQUAL(64, pattern[3][7].getPot(0));

    // Other tracks/steps should be unaffected
    TEST_ASSERT_FALSE(pattern[0][0].getSwitch());
    TEST_ASSERT_FALSE(pattern[7][15].getSwitch());
}

void test_pattern_get_track() {
    Pattern pattern;

    // Get track reference and modify
    Track& track = pattern.getTrack(2);
    track[5].setSwitch(true);
    track[5].setPot(1, 127);

    // Verify via direct access
    TEST_ASSERT_TRUE(pattern[2][5].getSwitch());
    TEST_ASSERT_EQUAL(127, pattern[2][5].getPot(1));
}

void test_pattern_clear() {
    Pattern pattern;

    // Set some events
    pattern[0][0].setSwitch(true);
    pattern[3][7].setPot(0, 100);
    pattern[7][15].setSwitch(true);

    TEST_ASSERT_TRUE(pattern.hasActiveEvents());

    // Clear pattern
    pattern.clear();

    // All events should be empty
    TEST_ASSERT_FALSE(pattern.hasActiveEvents());
    for (uint8_t t = 0; t < 8; t++) {
        TEST_ASSERT_FALSE(pattern.getTrack(t).hasActiveEvents());
    }
}

void test_pattern_has_active_events() {
    Pattern pattern;

    TEST_ASSERT_FALSE(pattern.hasActiveEvents());

    // Add event to one track
    pattern[2][8].setSwitch(true);
    TEST_ASSERT_TRUE(pattern.hasActiveEvents());

    // Add events to multiple tracks
    pattern[5][3].setSwitch(true);
    pattern[7][12].setSwitch(true);
    TEST_ASSERT_TRUE(pattern.hasActiveEvents());

    // Clear and verify
    pattern.clear();
    TEST_ASSERT_FALSE(pattern.hasActiveEvents());
}

void test_pattern_index_wrapping() {
    Pattern pattern;

    // Set event on track 3
    pattern[3][5].setSwitch(true);

    // Access with wrapped index (3 + 8 = 11, should wrap to 3)
    TEST_ASSERT_TRUE(pattern[11][5].getSwitch());

    // Verify mask behavior
    TEST_ASSERT_TRUE(pattern[3 & 0x07][5].getSwitch());
}

void test_pattern_get_num_tracks() {
    TEST_ASSERT_EQUAL(8, Pattern::getNumTracks());
}

void test_pattern_memory_size() {
    // Pattern should be 8 tracks × 64 bytes = 512 bytes
    TEST_ASSERT_EQUAL(512, sizeof(Pattern));
}

void test_pattern_multiple_events_per_track() {
    Pattern pattern;

    // Program a full beat on track 0
    for (uint8_t i = 0; i < 16; i += 4) {
        pattern[0][i].setSwitch(true);
        pattern[0][i].setPot(0, 100 + i);  // Varying velocity
    }

    // Verify all events
    for (uint8_t i = 0; i < 16; i++) {
        if (i % 4 == 0) {
            TEST_ASSERT_TRUE(pattern[0][i].getSwitch());
            TEST_ASSERT_EQUAL(100 + i, pattern[0][i].getPot(0));
        } else {
            TEST_ASSERT_FALSE(pattern[0][i].getSwitch());
        }
    }
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_pattern_initialization);
    RUN_TEST(test_pattern_track_access);
    RUN_TEST(test_pattern_get_track);
    RUN_TEST(test_pattern_clear);
    RUN_TEST(test_pattern_has_active_events);
    RUN_TEST(test_pattern_index_wrapping);
    RUN_TEST(test_pattern_get_num_tracks);
    RUN_TEST(test_pattern_memory_size);
    RUN_TEST(test_pattern_multiple_events_per_track);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
