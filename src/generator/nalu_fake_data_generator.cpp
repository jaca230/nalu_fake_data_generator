#include "generator/nalu_fake_data_generator.h"
#include <cstring>
#include <chrono>
#include <algorithm>

NaluFakeDataGenerator::NaluFakeDataGenerator()
    : config_(), current_event_index_(0), last_event_time_(0), rng_(std::random_device{}()) {
    updateEventInfo();
}

NaluFakeDataGenerator::NaluFakeDataGenerator(const NaluFakeDataGeneratorConfig& config)
    : config_(config), current_event_index_(config.start_index), last_event_time_(0), rng_(std::random_device{}()) {
    updateEventInfo();
}

std::unique_ptr<NaluEvent> NaluFakeDataGenerator::getNextEvent() {
    auto channels = config_.active_channels();
    size_t expected_packets = channels.size() * config_.num_windows;

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

    // For each channel, generate full waveform island (num_windows * 32 samples)
    for (auto channel : channels) {
        auto gen_it = channel_generators_.find(channel);
        std::vector<uint16_t> full_island;

        if (gen_it != channel_generators_.end()) {
            full_island = gen_it->second(config_.num_windows * 32);
        } else if (default_generator_) {
            full_island = default_generator_(config_.num_windows * 32);
        } else {
            // Fallback: generate zeroed waveform
            full_island.resize(config_.num_windows * 32, 0);
        }

        // Chop full waveform into 32-sample packets per window
        for (int window = 0; window < config_.num_windows; ++window) {
            const uint16_t* window_samples = full_island.data() + window * 32;

            auto packet = generatePacket(
                static_cast<uint8_t>(channel),
                event_reference_time,
                static_cast<uint16_t>(window),
                window_samples
            );
            event->add_packet(packet);
        }
    }

    last_event_time_ = event_reference_time;
    return event;
}


NaluCollectorTimingData NaluFakeDataGenerator::getNextTimingData() {
    NaluCollectorTimingData timing_data;

    timing_data.collection_cycle_index = 12345ULL;
    timing_data.collection_cycle_timestamp_ns = 9876543210LL;
    timing_data.udp_time = 123.456;
    timing_data.parse_time = 234.567;
    timing_data.event_time = 345.678;
    timing_data.total_time = 456.789;
    timing_data.data_processed = 123456ULL;
    timing_data.data_rate = 789.123;

    return timing_data;
}



NaluPacket NaluFakeDataGenerator::generatePacket(uint8_t channel, uint32_t trigger_time, uint16_t physical_position, const uint16_t* window_samples) {
    uint8_t packed_samples[64];

    // Pack each 12-bit sample into 2 bytes: 
    // byte1 = upper 4 bits in lower nibble, upper nibble zeroed
    // byte2 = lower 8 bits
    for (size_t i = 0; i < 32; ++i) {
        uint16_t adc = window_samples[i] & 0x0FFF; // 12-bit mask
        packed_samples[2 * i]     = static_cast<uint8_t>((adc >> 8) & 0x0F);
        packed_samples[2 * i + 1] = static_cast<uint8_t>(adc & 0xFF);
    }

    return NaluPacket(
        config_.packet_header,
        channel,
        trigger_time,
        0, // logical position unused
        physical_position,
        packed_samples,
        config_.packet_footer,
        config_.packet_info
    );
}

void NaluFakeDataGenerator::updateConfig(const NaluFakeDataGeneratorConfig& new_config) {
    config_ = new_config;
    updateEventInfo();
}

void NaluFakeDataGenerator::updateEventInfo() {
    // If no default generator, set one now (random noise)
    if (!default_generator_) {
        default_generator_ = [](uint16_t length) {
            std::vector<uint16_t> trace(length);
            static thread_local std::mt19937 local_rng(std::random_device{}());
            static thread_local std::uniform_int_distribution<uint16_t> dist(0, 0xFFF);
            std::generate(trace.begin(), trace.end(), []() { return dist(local_rng); });
            return trace;
        };
    }

    // For each active channel, if no generator exists, assign default generator
    for (uint8_t channel : config_.active_channels()) {
        if (channel_generators_.find(channel) == channel_generators_.end()) {
            channel_generators_[channel] = default_generator_;
        }
    }
}

uint32_t NaluFakeDataGenerator::getSimulatedTimestamp() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto ns_since_epoch = duration_cast<nanoseconds>(now.time_since_epoch()).count();
    uint64_t ticks = ns_since_epoch / CLOCK_PERIOD_NS;
    return static_cast<uint32_t>(ticks % (COUNTER_MAX + 1));
}

void NaluFakeDataGenerator::setGenerator(uint8_t channel, ChannelWaveformGenerator generator) {
    channel_generators_[channel] = std::move(generator);
}

void NaluFakeDataGenerator::setDefaultGenerator(ChannelWaveformGenerator generator) {
    default_generator_ = std::move(generator);
}
