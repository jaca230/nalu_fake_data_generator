#ifndef NALU_FAKE_DATA_GENERATOR_H
#define NALU_FAKE_DATA_GENERATOR_H

#include "nalu_event.h"
#include "nalu_collector_timing_data.h"
#include "nalu_fake_data_generator_config.h"
#include <memory>
#include <random>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <vector>

class NaluFakeDataGenerator {
public:
    using ChannelWaveformGenerator = std::function<std::vector<uint16_t>(uint16_t window)>;

    NaluFakeDataGenerator();
    explicit NaluFakeDataGenerator(const NaluFakeDataGeneratorConfig& config);

    // Generate and return the next event
    std::unique_ptr<NaluEvent> getNextEvent();

    // Get the next timing data for the event collector
    NaluCollectorTimingData getNextTimingData();

    // Update configuration
    void updateConfig(const NaluFakeDataGeneratorConfig& new_config);

    // Set waveform generator for a specific channel
    void setGenerator(uint8_t channel, ChannelWaveformGenerator generator);

    // Set default waveform generator used for channels without a specific generator
    void setDefaultGenerator(ChannelWaveformGenerator generator);

private:
    NaluFakeDataGeneratorConfig config_;
    uint32_t current_event_index_;
    uint32_t last_event_time_;  // May be unused, kept for compatibility

    // RNG members
    std::mt19937 rng_;
    std::uniform_int_distribution<uint8_t> dist_{0, 255};

    // Map of per-channel waveform generators
    std::unordered_map<uint8_t, ChannelWaveformGenerator> channel_generators_;

    // Default waveform generator function
    ChannelWaveformGenerator default_generator_;

    // Helper methods
    NaluPacket generatePacket(uint8_t channel, uint32_t trigger_time, uint16_t physical_position, const uint16_t* window_samples);
    void updateEventInfo();

    // Timestamp simulation constants
    static constexpr uint32_t CLOCK_PERIOD_NS = 32;               // 32 ns per tick
    static constexpr uint32_t COUNTER_MAX = (1 << 24) - 1;        // 24-bit max counter

    // Generate a simulated timestamp based on steady_clock, wrapping at 24 bits
    uint32_t getSimulatedTimestamp();
};

#endif // NALU_FAKE_DATA_GENERATOR_H
