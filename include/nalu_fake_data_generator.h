#ifndef NALU_FAKE_DATA_GENERATOR_H
#define NALU_FAKE_DATA_GENERATOR_H

#include <vector>
#include <cstdint>
#include <random>

struct NaluGeneratorConfig {
  int num_channels = 8;
  int num_windows = 1;
  int packets_per_event = 2;
};

class NaluFakeDataGenerator {
public:
  explicit NaluFakeDataGenerator(const NaluGeneratorConfig& config);

  std::vector<uint8_t> get_next_event();

private:
  NaluGeneratorConfig config_;
  std::mt19937 rng_;

  std::vector<uint8_t> generate_event_header();
  std::vector<uint8_t> generate_packet();
  std::vector<uint8_t> generate_event_footer();

  uint32_t current_event_index_ = 0;
};

#endif // NALU_FAKE_DATA_GENERATOR_H
