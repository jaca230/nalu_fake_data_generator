#ifndef NALU_FAKE_DATA_GENERATOR_CONFIG_H
#define NALU_FAKE_DATA_GENERATOR_CONFIG_H

#include <cstdint>
#include <vector>

struct NaluFakeDataGeneratorConfig {
    // Event configuration
    uint16_t event_header = 0xBBBB;
    uint16_t event_footer = 0xEEEE;
    uint8_t event_info = 0;
    uint32_t start_index = 0;
    uint16_t packet_size = 80; // Default NaluPacket size
    
    // Channel configuration
    uint64_t channel_mask = 0x1; // Default to channel 0
    uint8_t num_windows = 1;
    
    // Packet configuration
    uint16_t packet_header = 0xAAAA;
    uint16_t packet_footer = 0xFFFF;
    uint8_t packet_info = 0;
    
    // Data generation
    bool randomize_samples = true;
    uint8_t fixed_sample_value = 0x55;
    
    // Validation
    std::vector<int> active_channels() const;
};

#endif // NALU_FAKE_DATA_GENERATOR_CONFIG_H