#include <unity.h>
#include "../src/core/Event.h"

void test_event_constructor() {
    Event e(true, 64, 32, 16, 8);

    TEST_ASSERT_TRUE(e.getSwitch());
    TEST_ASSERT_EQUAL(64, e.getPot(0));
    TEST_ASSERT_EQUAL(32, e.getPot(1));
    TEST_ASSERT_EQUAL(16, e.getPot(2));
    TEST_ASSERT_EQUAL(8, e.getPot(3));
}

void test_event_switch_toggle() {
    Event e;

    TEST_ASSERT_FALSE(e.getSwitch());
    e.toggleSwitch();
    TEST_ASSERT_TRUE(e.getSwitch());
    e.toggleSwitch();
    TEST_ASSERT_FALSE(e.getSwitch());
}

void test_event_pot_range() {
    Event e;

    // Set and verify all pots
    e.setPot(0, 127);
    e.setPot(1, 63);
    e.setPot(2, 31);
    e.setPot(3, 15);

    TEST_ASSERT_EQUAL(127, e.getPot(0));
    TEST_ASSERT_EQUAL(63, e.getPot(1));
    TEST_ASSERT_EQUAL(31, e.getPot(2));
    TEST_ASSERT_EQUAL(15, e.getPot(3));
}

void test_event_pot_overflow() {
    Event e;

    // Values > 127 should be masked to 7 bits
    e.setPot(0, 255);  // Should become 127
    TEST_ASSERT_EQUAL(127, e.getPot(0));

    e.setPot(1, 200);  // Should become 200 & 0x7F = 72
    TEST_ASSERT_EQUAL(72, e.getPot(1));
}

void test_event_is_empty() {
    Event e;

    TEST_ASSERT_TRUE(e.isEmpty());

    e.setSwitch(true);
    TEST_ASSERT_FALSE(e.isEmpty());

    e.clear();
    TEST_ASSERT_TRUE(e.isEmpty());
}

void test_event_raw_data() {
    Event e1(true, 100, 50, 25, 10);
    uint32_t raw = e1.getRaw();

    Event e2;
    e2.setRaw(raw);

    TEST_ASSERT_EQUAL(e1.getSwitch(), e2.getSwitch());
    TEST_ASSERT_EQUAL(e1.getPot(0), e2.getPot(0));
    TEST_ASSERT_EQUAL(e1.getPot(1), e2.getPot(1));
    TEST_ASSERT_EQUAL(e1.getPot(2), e2.getPot(2));
    TEST_ASSERT_EQUAL(e1.getPot(3), e2.getPot(3));
}

void test_event_memory_size() {
    // Event should fit in 4 bytes (uint32_t)
    TEST_ASSERT_EQUAL(4, sizeof(Event));
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_event_constructor);
    RUN_TEST(test_event_switch_toggle);
    RUN_TEST(test_event_pot_range);
    RUN_TEST(test_event_pot_overflow);
    RUN_TEST(test_event_is_empty);
    RUN_TEST(test_event_raw_data);
    RUN_TEST(test_event_memory_size);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}
