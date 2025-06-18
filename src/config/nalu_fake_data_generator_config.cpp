#include "config/nalu_fake_data_generator_config.h"
#include <vector>

std::vector<int> NaluFakeDataGeneratorConfig::active_channels() const {
    std::vector<int> channels;
    uint64_t mask = channel_mask;
    for (int i = 0; i < 64; ++i) {
        if (mask & 0x1) {
            channels.push_back(i);
        }
        mask >>= 1;
    }
    return channels;
}