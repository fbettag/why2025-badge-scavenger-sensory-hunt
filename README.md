# WHY2025 Scavenger Sensory Hunt

An interactive IoT scavenger hunt game for the WHY2025 badge that transforms environmental exploration into an engaging sensory adventure. Using the BME690 environmental sensor and BMI270 IMU, players complete challenges based on real-world conditions and physical interactions.

## Features

- **Environmental Quests**: Detect rain, cold, darkness, and air quality changes
- **Motion Challenges**: Shake, tilt, and move your badge to complete tasks
- **LoRa Networking**: Find and connect with other badge holders nearby
- **Persistent Progress**: Save your quest state to continue later
- **Debug Mode**: Monitor sensor data and quest states in real-time

## Hardware Requirements

- WHY2025 badge with ESP32-S3
- BME690 Environmental Sensor (I2C)
- BMI270 6-axis IMU (I2C)
- RA-01H LoRa module (SPI)
- 720x720 IPS LCD display
- MicroSD card slot
- Custom keyboard PCB

## Software Requirements

- ESP-IDF v6.0 or later
- Docker with ESP-IDF development container (recommended)
- Python 3.8+ for tools

## Project Structure

```
scavenger-sensory-hunt-why2025/
├── firmware/
│   └── scavenger_game/
│       ├── components/         # Custom components
│       │   ├── sensors/        # BME690 & BMI270 drivers
│       │   ├── quest_engine/   # Quest logic and state
│       │   ├── display/        # LCD interface
│       │   ├── lora/           # LoRa communication
│       │   ├── storage/        # SD/NVS persistence
│       │   └── debug/          # Debug utilities
│       └── main/               # Main application
├── quests/                     # Quest definitions (JSON)
├── data/                       # Sensor data logs
├── tools/                      # Helper scripts
└── docs/                       # Documentation
```

## Building the Project

### Using Docker (Recommended)

1. Open the project in VS Code with the Dev Container extension
2. The container will automatically set up ESP-IDF v6.0
3. Build the project:
   ```bash
   . /opt/esp/idf/export.sh
   idf.py build
   ```

### Manual Setup

1. Install ESP-IDF v6.0:
   ```bash
   git clone -b release/v6.0 --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   ./install.sh esp32s3
   . ./export.sh
   ```

2. Build the project:
   ```bash
   cd scavenger-sensory-hunt-why2025
   idf.py build
   ```

## Flashing

1. Connect the WHY2025 badge via USB
2. Flash the firmware:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

## Configuration

### Sensor Thresholds

Edit the thresholds in `components/sensors/sensor_manager.c`:
- `RAIN_HUMIDITY_THRESHOLD`: 85% (detect high humidity)
- `COLD_TEMP_THRESHOLD`: 15°C (detect cold areas)
- `SMOKE_VOC_THRESHOLD`: 400 (detect air quality changes)
- `MOVEMENT_THRESHOLD`: 1.5g (detect shaking)
- `TILT_THRESHOLD`: 30° (detect tilting)

### Quest Configuration

Modify `quests/quest_map.json` to add or customize quests:
```json
{
  "id": 1,
  "name": "Rain Dancer",
  "description": "Find a rainy spot and dance!",
  "trigger": "humidity",
  "threshold": 85,
  "points": 100
}
```

## Usage

1. **Power On**: The badge will initialize and show the welcome screen
2. **Navigate**: Use the keyboard to browse available quests
3. **Activate Quest**: Select a quest to start tracking
4. **Complete Challenges**: Follow quest hints to trigger sensors
5. **View Progress**: Check your score and completed quests
6. **Debug Mode**: Press 'D' to toggle sensor data display

## Development

### Adding New Sensors

1. Create driver files in `components/sensors/`
2. Update `sensor_manager.h` with new data fields
3. Implement reading logic in `sensor_manager.c`
4. Add trigger detection functions

### Creating New Quests

1. Define quest in `quests/quest_map.json`
2. Add trigger type to `quest_system.h`
3. Implement trigger check in `quest_check_trigger()`
4. Update display to show quest details

### Debugging

Enable debug output:
```bash
idf.py menuconfig
# Component config → Log output → Default log verbosity → Debug
```

Monitor sensor data:
```c
debug_manager_log_sensor_data();
debug_manager_log_quest_state();
```

## Troubleshooting

### Sensor Not Detected
- Check I2C connections (SDA/SCL)
- Verify pull-up resistors (4.7kΩ)
- Run I2C scanner to find addresses

### LoRa Not Working
- Verify SPI connections
- Check antenna attachment
- Ensure correct frequency settings

### SD Card Issues
- Format as FAT32
- Check card compatibility (max 32GB)
- Verify SPI mode connections

## Contributing

1. Fork the repository
2. Create a feature branch
3. Test thoroughly on hardware
4. Submit a pull request

## License

This project is licensed under the MIT License - see LICENSE file for details.

## Responsible Use

This project includes smoke detection capabilities. Please:
- Use responsibly and ethically
- Respect privacy and consent
- Follow local laws and regulations
- Do not use for surveillance
- Focus on harm reduction and safety

## Project Status

🚧 **Pre-release**: Hardware drivers are placeholders pending WHY2025 badge availability.

- ✅ Quest system and game logic
- ✅ ML infrastructure ready
- ✅ Data collection tools
- ⏳ Actual sensor drivers (BME690, BMI270)
- ⏳ Display driver (720x720 LCD)
- ⏳ LoRa implementation

## Acknowledgments

- WHY2025 badge team for hardware design
- ESP-IDF community for excellent documentation
- Sensirion for BME680 driver examples
- Bosch for BMI270 reference implementation

## Contributors

<!-- ALL-CONTRIBUTORS-LIST:START -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.