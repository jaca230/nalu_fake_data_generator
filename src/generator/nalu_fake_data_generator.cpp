#include "nalu_fake_data_generator.h"
#include <cstring>
#include <chrono>

NaluFakeDataGenerator::NaluFakeDataGenerator()
    : config_(), current_event_index_(0), last_event_time_(0),
      rng_(std::random_device{}()) {
    updateEventInfo();
}

NaluFakeDataGenerator::NaluFakeDataGenerator(const NaluFakeDataGeneratorConfig& config)
    : config_(config), current_event_index_(config.start_index), last_event_time_(0),
      rng_(std::random_device{}()) {
    updateEventInfo();
}

std::unique_ptr<NaluEvent> NaluFakeDataGenerator::getNextEvent() {
    auto channels = config_.active_channels();
    size_t expected_packets = channels.size() * config_.num_windows;

    // Get the simulated 24-bit hardware timestamp once for the whole event
    uint32_t event_reference_time = getSimulatedTimestamp();

    auto event = std::make_unique<NaluEvent>(
        config_.event_header,
        config_.event_info,
        current_event_index_++,
        event_reference_time,
        config_.packet_size,
        0,
        config_.event_footer,
        static_cast<uint16_t>(expected_packets + 1000),
        config_.channel_mask,
        config_.num_windows
    );

    // For each window and channel, generate a packet with the SAME timestamp
    for (int window = 0; window < config_.num_windows; ++window) {
        for (auto channel : channels) {
            auto packet = generatePacket(static_cast<uint8_t>(channel), event_reference_time, static_cast<uint16_t>(window));
            event->add_packet(packet);
        }
    }

    // Update last_event_time_ if desired (kept for compatibility)
    last_event_time_ = event_reference_time;

    return event;
}


NaluPacket NaluFakeDataGenerator::generatePacket(uint8_t channel, uint32_t trigger_time, uint16_t physical_position) {
    uint8_t samples[64];

    if (config_.randomize_samples) {
        for (auto& sample : samples) {
            sample = dist_(rng_);
        }
    } else {
        memset(samples, config_.fixed_sample_value, sizeof(samples));
    }

    return NaluPacket(
        config_.packet_header,
        channel,
        trigger_time,
        0,  // logical position (unchanged)
        physical_position,
        samples,
        config_.packet_footer,
        config_.packet_info
    );
}

void NaluFakeDataGenerator::updateConfig(const NaluFakeDataGeneratorConfig& new_config) {
    config_ = new_config;
    updateEventInfo();
}

void NaluFakeDataGenerator::updateEventInfo() {
    // Currently empty — add logic if needed
}

uint32_t NaluFakeDataGenerator::getSimulatedTimestamp() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto ns_since_epoch = duration_cast<nanoseconds>(now.time_since_epoch()).count();

    // Convert to 32 ns clock ticks
    uint64_t ticks = ns_since_epoch / CLOCK_PERIOD_NS;

    // Wrap counter at 24 bits
    return static_cast<uint32_t>(ticks % (COUNTER_MAX + 1));
}
