#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

/**
 * Event - The fundamental unit of musical data in GRUVBOK
 *
 * Represents a single "snapshot" captured when user presses a button.
 * Contains:
 * - Switch: on/off state (1 bit)
 * - Pot[4]: Four slider values 0-127 (7 bits each = 28 bits)
 *
 * Total: 29 bits, stored in uint32_t for efficiency
 *
 * Memory layout (32-bit word):
 * [31:29] unused
 * [28:28] switch (1 bit)
 * [27:21] pot0 (7 bits)
 * [20:14] pot1 (7 bits)
 * [13:7]  pot2 (7 bits)
 * [6:0]   pot3 (7 bits)
 */
class Event {
private:
  uint32_t data;

  static constexpr uint32_t SWITCH_MASK = 0x10000000;  // Bit 28
  static constexpr uint32_t POT0_MASK   = 0x0FE00000;  // Bits 21-27
  static constexpr uint32_t POT1_MASK   = 0x001FC000;  // Bits 14-20
  static constexpr uint32_t POT2_MASK   = 0x00003F80;  // Bits 7-13
  static constexpr uint32_t POT3_MASK   = 0x0000007F;  // Bits 0-6

public:
  Event() : data(0) {}

  Event(bool sw, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3) {
    setSwitch(sw);
    setPot(0, p0);
    setPot(1, p1);
    setPot(2, p2);
    setPot(3, p3);
  }

  // Switch access
  inline bool getSwitch() const {
    return (data & SWITCH_MASK) != 0;
  }

  inline void setSwitch(bool value) {
    if (value) {
      data |= SWITCH_MASK;
    } else {
      data &= ~SWITCH_MASK;
    }
  }

  inline void toggleSwitch() {
    data ^= SWITCH_MASK;
  }

  // Pot access (index 0-3)
  inline uint8_t getPot(uint8_t index) const {
    switch(index) {
      case 0: return (data & POT0_MASK) >> 21;
      case 1: return (data & POT1_MASK) >> 14;
      case 2: return (data & POT2_MASK) >> 7;
      case 3: return (data & POT3_MASK);
      default: return 0;
    }
  }

  inline void setPot(uint8_t index, uint8_t value) {
    value &= 0x7F;  // Ensure 7-bit value
    switch(index) {
      case 0:
        data = (data & ~POT0_MASK) | ((uint32_t)value << 21);
        break;
      case 1:
        data = (data & ~POT1_MASK) | ((uint32_t)value << 14);
        break;
      case 2:
        data = (data & ~POT2_MASK) | ((uint32_t)value << 7);
        break;
      case 3:
        data = (data & ~POT3_MASK) | value;
        break;
    }
  }

  // Raw data access for serialization
  inline uint32_t getRaw() const { return data; }
  inline void setRaw(uint32_t raw) { data = raw; }

  // Check if event is empty (all zeros)
  inline bool isEmpty() const { return data == 0; }

  // Clear event
  inline void clear() { data = 0; }
};

#endif // EVENT_H
