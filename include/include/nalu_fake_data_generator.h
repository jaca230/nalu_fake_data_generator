#ifndef NALU_FAKE_DATA_GENERATOR_H
#define NALU_FAKE_DATA_GENERATOR_H

#include "nalu_event.h"
#include "nalu_fake_data_generator_config.h"
#include <memory>
#include <random>
#include <cstdint>

class NaluFakeDataGenerator {
public:
    NaluFakeDataGenerator();
    explicit NaluFakeDataGenerator(const NaluFakeDataGeneratorConfig& config);

    // Generate and return the next event
    std::unique_ptr<NaluEvent> getNextEvent();

    // Update configuration
    void updateConfig(const NaluFakeDataGeneratorConfig& new_config);

private:
    NaluFakeDataGeneratorConfig config_;
    uint32_t current_event_index_;
    uint32_t last_event_time_;  // May be unused, kept for compatibility

    // RNG members
    std::mt19937 rng_;
    std::uniform_int_distribution<uint8_t> dist_{0, 255};

    // Helper methods
    NaluPacket generatePacket(uint8_t channel, uint32_t trigger_time, uint16_t physical_position);
    void updateEventInfo();

    // Timestamp simulation constants
    static constexpr uint32_t CLOCK_PERIOD_NS = 32;               // 32 ns per tick
    static constexpr uint32_t COUNTER_MAX = (1 << 24) - 1;        // 24-bit max counter

    // Generate a simulated timestamp based on steady_clock, wrapping at 24 bits
    uint32_t getSimulatedTimestamp();
};

#endif // NALU_FAKE_DATA_GENERATOR_H
