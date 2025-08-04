# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an interactive IoT scavenger hunt game for the WHY2025 badge, an ESP32-S3 based device with environmental sensors (BME690, BMI270), LoRa communication, and a 720x720 IPS display. The game uses sensor data to trigger quests based on real-world environmental conditions.

## Build and Development Commands

### Initial Setup (in devcontainer)
```bash
# Set up ESP-IDF environment
. /opt/esp/idf/export.sh

# Build the firmware
cd firmware/scavenger_game
idf.py build

# Flash to device (replace port as needed)
idf.py -p /dev/ttyUSB0 flash monitor

# Just monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

### Python Tools
```bash
# Lint Python code
flake8 tools/*.py

# Format Python code
black tools/*.py

# Run VOC data collection
./tools/voc_data_collector.py /dev/ttyUSB0 --label normal --duration 60

# Visualize collected data
./tools/voc_data_visualizer.py data/training_data.csv
```

### Testing
```bash
# Run unit tests if available
cd firmware/scavenger_game
idf.py test

# Clean build
idf.py fullclean
```

## Architecture Overview

### Firmware Structure (ESP-IDF Components)
The firmware follows ESP-IDF's component architecture at `firmware/scavenger_game/`:

- **sensors/** - Hardware drivers for BME690 (environmental) and BMI270 (IMU) sensors
  - `sensor_manager.c/.h` - Central sensor management and thresholds
  - `bme690_driver.c/.h` - Temperature, humidity, pressure, VOC readings
  - `bmi270_driver.c/.h` - Accelerometer and gyroscope for motion detection

- **quest_engine/** - Core game logic
  - `quest_system.c/.h` - Quest state machine, trigger detection, progress tracking
  - Loads quest definitions from `quests/quest_map.json`
  - Manages quest completion, scoring, and achievements

- **ml_model/** - TinyML integration for smoke classification
  - `ml_model_manager.c/.h` - TensorFlow Lite Micro runtime
  - Distinguishes cigarette vs herbal smoke using VOC patterns
  - Model deployed as C array after training

- **lora/** - LoRa networking via RA-01H module
  - `lora_manager.c/.h` - Badge discovery and P2P communication
  - Proximity-based quests and social features

- **display/** - 720x720 LCD interface
  - `display_manager.c/.h` - UI rendering, quest display, sensor visualization

- **storage/** - Persistent data management
  - `storage_manager.c/.h` - SD card and NVS for save games
  - Quest progress and high scores

- **debug/** - Development utilities
  - `debug_manager.c/.h` - Real-time sensor monitoring, logging

### Quest System
Quests are defined in `quests/quest_map.json` with trigger types:
- **Sensor triggers**: humidity, temperature, darkness, cigarette/herbal smoke, movement, tilt
- **Proximity triggers**: LoRa badge discovery
- **Combo triggers**: Complete multiple quests

Each quest has thresholds, target counts, points, and hints. The quest engine monitors sensor data against active quest triggers.

### ML Smoke Detection
The project includes infrastructure for training a TinyML model to classify smoke types:
1. Collect VOC data using `tools/voc_data_collector.py`
2. Train model using TensorFlow (see `docs/ML_TRAINING_GUIDE.md`)
3. Deploy as quantized TFLite model in firmware

## Key Sensor Thresholds
- Rain detection: humidity > 85%
- Cold detection: temperature < 15°C
- Cigarette smoke: VOC 350-600
- Herbal smoke: VOC > 600
- Movement: acceleration > 1.5g
- Tilt: angle > 30°

## Development Notes
- Hardware drivers are placeholders pending actual WHY2025 badge availability
- Use debug mode ('D' key) to monitor real-time sensor values
- Test thoroughly on actual hardware before committing sensor-related changes
- Follow ESP-IDF coding conventions in C code
- Python tools should follow PEP 8 (use black/flake8)