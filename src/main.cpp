#include "nalu_fake_data_generator.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm> // for std::min

int main() {
    // Configuration setup
    NaluFakeDataGeneratorConfig config;
    config.num_windows = 4;
    config.channel_mask = 0x3; // channels 0 and 1
    config.randomize_samples = false; // we'll override the generator

    NaluFakeDataGenerator generator(config);

    // Define a square wave generator that creates a waveform with alternating blocks of low/high
    auto square_wave_generator = [](uint16_t length) -> std::vector<uint16_t> {
        std::vector<uint16_t> waveform(length);
        // Square wave with period = 32 samples, half period = 16 samples
        // low level = 0x100, high level = 0xF00
        for (uint16_t i = 0; i < length; ++i) {
            waveform[i] = ( (i / 16) % 2 == 0 ) ? 0x0100 : 0x0F00;
        }
        return waveform;
    };

    // Assign the square wave generator to all active channels
    for (uint8_t channel : config.active_channels()) {
        generator.setGenerator(channel, square_wave_generator);
    }

    constexpr int num_events_to_generate = 3;

    for (int i = 0; i < num_events_to_generate; ++i) {
        auto event = generator.getNextEvent();

        size_t event_size = event->get_size();
        std::unique_ptr<char[]> buffer(new char[event_size]);
        event->serialize_to_buffer(buffer.get());

        // Print event header info and basic stats
        std::cout << "Event " << event->header.index
                  << ", Packets: " << event->header.num_packets
                  << ", Size: " << event_size << " bytes\n";

        // Print first 252 bytes (or less)
        size_t bytes_to_print = std::min(event_size, size_t(252));
        std::cout << "Serialized Event Buffer (First " << bytes_to_print << " bytes):\n";

        for (size_t i = 0; i < bytes_to_print; i += 4) {
            std::cout << std::hex;
            for (size_t j = 0; j < 4 && (i + j) < bytes_to_print; ++j) {
                std::cout << std::setw(2) << std::setfill('0')
                          << (static_cast<unsigned int>(buffer[i + j]) & 0xFF) << " ";
            }
            std::cout << std::dec << "\n";
        }

        // Print last 50 bytes (or less)
        size_t last_bytes_to_print = std::min(event_size, size_t(50));
        std::cout << "Serialized Event Buffer (Last " << last_bytes_to_print << " bytes):\n";

        size_t start_last_section = event_size - last_bytes_to_print;
        for (size_t i = start_last_section; i < event_size; i += 4) {
            std::cout << std::hex;
            for (size_t j = 0; j < 4 && (i + j) < event_size; ++j) {
                std::cout << std::setw(2) << std::setfill('0')
                          << (static_cast<unsigned int>(buffer[i + j]) & 0xFF) << " ";
            }
            std::cout << std::dec << "\n";
        }

        // Optionally print detailed event info
        event->print_event_info();

        // Print creation timestamp in ms since steady_clock epoch (mostly for debugging)
        auto ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            event->get_creation_timestamp().time_since_epoch()).count();
        std::cout << "Creation Timestamp: " << ts_ms << " ms\n";

        // --- Print the timing data ---
        auto timing = generator.getNextTimingData();

        std::cout << "\nTiming Data:\n"
                  << "  Collection Cycle Index: " << timing.collection_cycle_index << "\n"
                  << "  Collection Cycle Timestamp (ns): " << timing.collection_cycle_timestamp_ns << "\n"
                  << "  UDP Time (s): " << timing.udp_time << "\n"
                  << "  Parse Time (s): " << timing.parse_time << "\n"
                  << "  Event Time (s): " << timing.event_time << "\n"
                  << "  Total Time (s): " << timing.total_time << "\n"
                  << "  Data Processed (bytes): " << timing.data_processed << "\n"
                  << "  Data Rate (MB/s): " << timing.data_rate << "\n\n";
    }

    return 0;
}
