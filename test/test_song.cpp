#include <unity.h>
#include "../src/core/Song.h"

void test_song_initialization() {
    Song song;

    // All patterns should be initialized and empty
    for (uint8_t m = 0; m < 15; m++) {
        for (uint8_t p = 0; p < 32; p++) {
            TEST_ASSERT_FALSE(song.getPattern(m, p).hasActiveEvents());
        }
    }
}

void test_song_pattern_access() {
    Song song;

    // Set event in Mode 1, Pattern 5, Track 3, Step 7
    song.getPattern(1, 5)[3][7].setSwitch(true);
    song.getPattern(1, 5)[3][7].setPot(0, 99);

    TEST_ASSERT_TRUE(song.getPattern(1, 5)[3][7].getSwitch());
    TEST_ASSERT_EQUAL(99, song.getPattern(1, 5)[3][7].getPot(0));

    // Other patterns should be unaffected
    TEST_ASSERT_FALSE(song.getPattern(0, 0)[0][0].getSwitch());
    TEST_ASSERT_FALSE(song.getPattern(14, 31)[7][15].getSwitch());
}

void test_song_clear() {
    Song song;

    // Set events in multiple modes/patterns
    song.getPattern(0, 0)[0][0].setSwitch(true);
    song.getPattern(5, 10)[3][8].setSwitch(true);
    song.getPattern(14, 31)[7][15].setSwitch(true);

    // Verify they exist
    TEST_ASSERT_TRUE(song.getPattern(0, 0)[0][0].getSwitch());
    TEST_ASSERT_TRUE(song.getPattern(5, 10)[3][8].getSwitch());
    TEST_ASSERT_TRUE(song.getPattern(14, 31)[7][15].getSwitch());

    // Clear song
    song.clear();

    // All patterns should be empty
    for (uint8_t m = 0; m < 15; m++) {
        for (uint8_t p = 0; p < 32; p++) {
            TEST_ASSERT_FALSE(song.getPattern(m, p).hasActiveEvents());
        }
    }
}

void test_song_mode_isolation() {
    Song song;

    // Set events in different modes at same pattern/track/step
    song.getPattern(0, 5)[2][10].setSwitch(true);
    song.getPattern(0, 5)[2][10].setPot(0, 50);

    song.getPattern(7, 5)[2][10].setSwitch(true);
    song.getPattern(7, 5)[2][10].setPot(0, 100);

    // Verify they're independent
    TEST_ASSERT_EQUAL(50, song.getPattern(0, 5)[2][10].getPot(0));
    TEST_ASSERT_EQUAL(100, song.getPattern(7, 5)[2][10].getPot(0));
}

void test_song_pattern_isolation() {
    Song song;

    // Set events in different patterns at same mode/track/step
    song.getPattern(3, 0)[4][8].setPot(0, 25);
    song.getPattern(3, 15)[4][8].setPot(0, 75);
    song.getPattern(3, 31)[4][8].setPot(0, 125);

    // Verify they're independent
    TEST_ASSERT_EQUAL(25, song.getPattern(3, 0)[4][8].getPot(0));
    TEST_ASSERT_EQUAL(75, song.getPattern(3, 15)[4][8].getPot(0));
    TEST_ASSERT_EQUAL(125, song.getPattern(3, 31)[4][8].getPot(0));
}

void test_song_index_wrapping() {
    Song song;

    // Set event at Mode 5, Pattern 10
    song.getPattern(5, 10)[0][0].setSwitch(true);

    // Access with wrapped indices
    // Mode wraps at 15 (using & 0x0F mask)
    // Pattern wraps at 32 (using & 0x1F mask)
    TEST_ASSERT_TRUE(song.getPattern(5, 10)[0][0].getSwitch());
    TEST_ASSERT_TRUE(song.getPattern(5 & 0x0F, 10 & 0x1F)[0][0].getSwitch());
}

void test_song_constants() {
    TEST_ASSERT_EQUAL(15, Song::getNumModes());
    TEST_ASSERT_EQUAL(32, Song::getNumPatterns());
}

void test_song_memory_size() {
    // Song should be 15 modes × 32 patterns × 512 bytes = 245,760 bytes
    size_t expectedSize = 15 * 32 * 512;
    TEST_ASSERT_EQUAL(expectedSize, Song::getMemorySize());

    // Verify actual sizeof matches expected
    TEST_ASSERT_EQUAL(expectedSize, sizeof(Song));
}

void test_song_full_pattern_programming() {
    Song song;
    song.clear();

    // Program a complete pattern across all tracks
    Pattern& pattern = song.getPattern(1, 0);

    // Four-on-the-floor kick (track 0)
    for (uint8_t i = 0; i < 16; i += 4) {
        pattern[0][i].setSwitch(true);
        pattern[0][i].setPot(0, 127);  // Max velocity
    }

    // Backbeat snare (track 1)
    pattern[1][4].setSwitch(true);
    pattern[1][4].setPot(0, 120);
    pattern[1][12].setSwitch(true);
    pattern[1][12].setPot(0, 120);

    // Verify kick pattern
    for (uint8_t i = 0; i < 16; i++) {
        if (i % 4 == 0) {
            TEST_ASSERT_TRUE(pattern[0][i].getSwitch());
            TEST_ASSERT_EQUAL(127, pattern[0][i].getPot(0));
        } else {
            TEST_ASSERT_FALSE(pattern[0][i].getSwitch());
        }
    }

    // Verify snare pattern
    TEST_ASSERT_TRUE(pattern[1][4].getSwitch());
    TEST_ASSERT_TRUE(pattern[1][12].getSwitch());
    TEST_ASSERT_FALSE(pattern[1][0].getSwitch());
}

void test_song_multi_mode_song() {
    Song song;
    song.clear();

    // Program Mode 0 (Pattern Sequencer)
    song.getPattern(0, 0)[0][0].setSwitch(true);
    song.getPattern(0, 0)[0][0].setPot(0, 10);  // Switch to pattern 10

    // Program Mode 1 (Drums) - Pattern 0
    song.getPattern(1, 0)[0][0].setSwitch(true);
    song.getPattern(1, 0)[0][0].setPot(0, 100);

    // Program Mode 1 (Drums) - Pattern 10
    song.getPattern(1, 10)[1][4].setSwitch(true);
    song.getPattern(1, 10)[1][4].setPot(0, 110);

    // Verify independence
    TEST_ASSERT_TRUE(song.getPattern(0, 0).hasActiveEvents());
    TEST_ASSERT_TRUE(song.getPattern(1, 0).hasActiveEvents());
    TEST_ASSERT_TRUE(song.getPattern(1, 10).hasActiveEvents());

    // Mode 2-14 should still be empty
    for (uint8_t m = 2; m < 15; m++) {
        TEST_ASSERT_FALSE(song.getPattern(m, 0).hasActiveEvents());
    }
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_song_initialization);
    RUN_TEST(test_song_pattern_access);
    RUN_TEST(test_song_clear);
    RUN_TEST(test_song_mode_isolation);
    RUN_TEST(test_song_pattern_isolation);
    RUN_TEST(test_song_index_wrapping);
    RUN_TEST(test_song_constants);
    RUN_TEST(test_song_memory_size);
    RUN_TEST(test_song_full_pattern_programming);
    RUN_TEST(test_song_multi_mode_song);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
