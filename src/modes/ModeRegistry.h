#ifndef MODEREGISTRY_H
#define MODEREGISTRY_H

#include "Mode.h"
#include "../core/Constants.h"
#include <stdint.h>

/**
 * ModeRegistry - Manages mode lifecycle and access
 *
 * Provides a safe, extensible way to manage mode instances.
 * Simplifies adding/removing modes without modifying sequencer code.
 *
 * Benefits:
 * - Type-safe mode access with bounds checking
 * - Automatic cleanup (RAII)
 * - Easy mode registration
 * - Null-safe iteration
 * - Clear ownership semantics
 */
class ModeRegistry {
private:
  static constexpr uint8_t MAX_MODES = GRUVBOK::Song::NUM_MODES;
  Mode* modes[MAX_MODES];

public:
  ModeRegistry() {
    // Initialize all slots to nullptr
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      modes[i] = nullptr;
    }
  }

  ~ModeRegistry() {
    // Clean up all registered modes
    clear();
  }

  /**
   * Register a mode at the specified index
   * Takes ownership of the mode pointer
   *
   * @param index Mode index (0-14)
   * @param mode Pointer to mode instance (must be heap-allocated)
   * @return true if successful, false if index invalid or slot occupied
   */
  bool registerMode(uint8_t index, Mode* mode) {
    if (index >= MAX_MODES) return false;
    if (mode == nullptr) return false;

    // If slot is occupied, delete old mode first
    if (modes[index] != nullptr) {
      delete modes[index];
    }

    modes[index] = mode;
    return true;
  }

  /**
   * Unregister and delete a mode
   *
   * @param index Mode index (0-14)
   */
  void unregisterMode(uint8_t index) {
    if (index >= MAX_MODES) return;

    if (modes[index] != nullptr) {
      delete modes[index];
      modes[index] = nullptr;
    }
  }

  /**
   * Get mode by index (const access)
   *
   * @param index Mode index (0-14)
   * @return Pointer to mode, or nullptr if not registered
   */
  const Mode* getMode(uint8_t index) const {
    if (index >= MAX_MODES) return nullptr;
    return modes[index];
  }

  /**
   * Get mode by index (mutable access)
   *
   * @param index Mode index (0-14)
   * @return Pointer to mode, or nullptr if not registered
   */
  Mode* getMode(uint8_t index) {
    if (index >= MAX_MODES) return nullptr;
    return modes[index];
  }

  /**
   * Check if mode is registered
   *
   * @param index Mode index (0-14)
   * @return true if mode exists at this index
   */
  bool hasMode(uint8_t index) const {
    if (index >= MAX_MODES) return false;
    return modes[index] != nullptr;
  }

  /**
   * Initialize all registered modes with scheduler
   *
   * @param scheduler MIDI scheduler instance
   */
  void initAll(MIDIScheduler* scheduler) {
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      if (modes[i] != nullptr) {
        modes[i]->init(scheduler);
      }
    }
  }

  /**
   * Clear all modes (delete and set to nullptr)
   */
  void clear() {
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      if (modes[i] != nullptr) {
        delete modes[i];
        modes[i] = nullptr;
      }
    }
  }

  /**
   * Count registered modes
   *
   * @return Number of non-null modes
   */
  uint8_t countRegisteredModes() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      if (modes[i] != nullptr) {
        count++;
      }
    }
    return count;
  }

  /**
   * Get mode name safely
   *
   * @param index Mode index (0-14)
   * @return Mode name, or "Empty" if not registered
   */
  const char* getModeName(uint8_t index) const {
    const Mode* mode = getMode(index);
    return (mode != nullptr) ? mode->getName() : "Empty";
  }

  /**
   * Iterator support - call function for each registered mode
   *
   * @param func Function to call with (index, mode*)
   */
  template<typename Func>
  void forEach(Func func) {
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      if (modes[i] != nullptr) {
        func(i, modes[i]);
      }
    }
  }

  /**
   * Const iterator support
   */
  template<typename Func>
  void forEach(Func func) const {
    for (uint8_t i = 0; i < MAX_MODES; i++) {
      if (modes[i] != nullptr) {
        func(i, modes[i]);
      }
    }
  }

  static constexpr uint8_t getMaxModes() { return MAX_MODES; }
};

#endif  // MODEREGISTRY_H
