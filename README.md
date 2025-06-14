# nalu_fake_data_generator

A modular C++ library for simulating hardware-like event data streams, modeled after the data format used in the [NaluEventCollector](https://github.com/jaca230/nalu_event_collector) system. This generator is intended for testing pipelines, backend services, and data visualization tools without requiring access to real hardware.

---

## Getting Started

### Build Requirements

- C++17 or later
- CMake ≥ 3.12

### Building

#### Clone the repository with submodules
```bash
git clone --recurse-submodules https://github.com/your-org/nalu_fake_data_generator.git
cd nalu_fake_data_generator
```

#### Build using the provided script
```bash
./scripts/build.sh
```
---

## Usage

### 1. Basic Configuration

Create a configuration and instantiate a generator:

```cpp
NaluFakeDataGeneratorConfig config;
config.num_windows = 4;
config.channel_mask = 0x3; // enables channels 0 and 1
config.randomize_samples = true;

NaluFakeDataGenerator generator(config);
```

### 2. Generate an Event

```cpp
auto event = generator.getNextEvent();
std::unique_ptr<char[]> buffer(new char[event->get_size()]);
event->serialize_to_buffer(buffer.get());
```

### 3. Access Timing Data

```cpp
auto timing_data = generator.getNextTimingData();
```

---

## Waveform Customization

You can assign waveform generators per channel:

```cpp
generator.setGenerator(0, [](uint16_t len) {
    std::vector<uint16_t> wave(len, 0x800); // Flat waveform
    return wave;
});
```

Or apply a square wave:

```cpp
auto square_wave = [](uint16_t len) {
    std::vector<uint16_t> wave(len);
    for (uint16_t i = 0; i < len; ++i)
        wave[i] = (i / 16) % 2 == 0 ? 0x0100 : 0x0F00;
    return wave;
};

generator.setGenerator(1, square_wave);
```


---

## Example

```cpp
for (int i = 0; i < 3; ++i) {
    auto event = generator.getNextEvent();
    auto timing = generator.getNextTimingData();

    std::cout << "Event " << event->header.index << "\n";
    event->print_event_info();
    std::cout << "Timing data: " << timing.total_time << "s, "
              << timing.data_rate << " MB/s\n";
}
```

---

## License

This project is provided under the MIT License.