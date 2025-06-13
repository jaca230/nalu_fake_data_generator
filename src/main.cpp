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
    config.randomize_samples = true;

    NaluFakeDataGenerator generator(config);

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
        std::cout << "Creation Timestamp: " << ts_ms << " ms\n\n";
    }

    return 0;
}
