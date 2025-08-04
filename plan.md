# WHY2025 Badge Project: Sensory Scavenger Hunt with TinyML Extension

## 📦 Project Overview

Create a mixed-reality scavenger hunt game for the WHY2025 badge. The game will use environmental and motion sensors to create interactive real-world quests. Later, an OTA update will ship a TinyML-powered model that enhances environmental awareness by detecting rain, cigarette smoke, and weed smoke.

---

## 🛠 Hardware Overview

| Component           | Description                                                   |
|--------------------|---------------------------------------------------------------|
| MCU                | ESP32-P4 (main) + ESP32-C6 (connectivity)                     |
| Display            | 4" 720×720 IPS LCD                                            |
| Keyboard           | Full QWERTY, haptic feedback, RGB backlight                   |
| Sensors            | Bosch BMI270 (IMU), Bosch BME690 (Temp, Humidity, Pressure, VOC) |
| Wireless           | Wi-Fi 6, Bluetooth 5, LoRa RA-01H                             |
| Storage            | 32MB Flash, microSD slot                                     |
| Expansion          | GPIOs, external speaker connector, camera port               |
| Power              | 2×18650 Li-ion batteries                                     |

---

## 📅 Phase 1: Scavenger Hunt Game (Baseline, No AI)

### 🎯 Goals
- Fully playable scavenger game using badge sensors
- Fun tasks based on environment and motion
- Basic networking between badges via LoRa

### ✅ Features
- Quest progression via local JSON save state
- Player input through the badge’s keyboard
- Puzzle and hint rendering on the screen
- Sensor-based triggers for location-specific tasks
- LoRa-based group puzzles or badge proximity unlocks

### 🧩 Sensor-Based Triggers
| Condition               | Trigger Logic                             | Sensor        |
|------------------------|-------------------------------------------|---------------|
| Movement / Shake       | IMU acceleration exceeds threshold        | BMI270        |
| Tilting / Orientation  | IMU tilt angle deviation                  | BMI270        |
| Rainy or damp location | Humidity > 85%                            | BME690        |
| Cold area              | Temperature < 15°C                        | BME690        |
| Covered / darkness     | Temp drop + humidity rise                 | BME690 (proxy)|
| Bad air (smoke hint)   | VOC value > baseline threshold            | BME690        |
| Group puzzle           | Nearby badge detected via LoRa ping       | ESP32-C6 + LoRa|

### 🛠 Tasks
- [ ] Implement UI/UX flow for game (badge screen + keyboard)
- [ ] Build quest logic and state machine
- [ ] Integrate BME690 and BMI270 sensor polling
- [ ] Define 5–10 location challenges using thresholds
- [ ] Implement LoRa badge-to-badge communication
- [ ] Build SD-based or flash-based quest state persistence
- [ ] Add debug/logging output toggle

---

## 🔁 Phase 2: TinyML OTA Upgrade (AI-Powered Detection)

### 🎯 Goals
- Enable detection of:
  - Rainy weather
  - Cigarette smoke
  - Weed smoke
- Ship model via firmware update
- Run lightweight inference onboard (ESP32‑P4)

### 🧪 Data Collection

| Class Name        | Sample Duration | Labeling Method |
|-------------------|------------------|------------------|
| Clean Air         | 5–10 mins        | Manual           |
| Rainy Conditions  | 5–10 mins        | Weather-verified |
| Cigarette Smoke   | 5–10 mins        | Manual           |
| Weed Smoke        | 5–10 mins        | Manual           |

**Sensor inputs:**
- VOC (gas)
- Temperature
- Humidity
- Pressure

**Logging format (JSON or CSV):**
```json
{
  "timestamp": 1690812345,
  "voc": 400,
  "temp": 21.4,
  "humidity": 92,
  "pressure": 1002,
  "label": "cigarette"
}
```

### 🧠 Model Training
- Normalize sensor input
- Try KNN, Decision Tree, or shallow NN
- Framework: TensorFlow Lite Micro / Edge Impulse
- Target size: < 20 KB, inference time < 50 ms

### 🛠 Tasks
- [ ] Build CLI or badge-based logger
- [ ] Collect labeled data samples
- [ ] Train + export model (.tflite or C header)
- [ ] Integrate model into firmware with inference loop
- [ ] Add result-to-quest interface (e.g. `quest.unlock('rain')`)
- [ ] OTA update or QR-triggered firmware loader

---

## 🧪 Test Plan

| Test | Method |
|------|--------|
| Sensor thresholds | Simulate with dummy data or real environment |
| LoRa p2p         | Test with multiple badges |
| Model inference  | Unit test with captured datasets |
| OTA update       | Manual test + checksum validation |

---

## 📁 Folder Structure Proposal

```
badge-hunt/
├── firmware/
│   ├── scavenger_game/
│   ├── ai_model_inference/
│   └── ota_updater/
├── data/
│   ├── raw_logs/
│   ├── labeled/
│   └── model/
├── tools/
│   ├── logger.py
│   └── dataset_trainer.ipynb
├── docs/
│   └── README.md
└── quests/
    └── quest_map.json
```

---

## 👥 Team Roles Suggestion

| Role                  | Responsibilities                              |
|-----------------------|-----------------------------------------------|
| Firmware Dev          | Sensor polling, quest engine, display/UI      |
| Game Designer         | Quest map, clues, progression logic            |
| AI Engineer           | Data pipeline, model training, validation      |
| OTA Integration Lead  | Firmware update logic and model deployment     |
| Field Tester          | Run tests onsite, log issues, verify detection |

---

## 📦 Deliverables

### v1.0 (Pre-Event)
- Functional scavenger hunt game with sensors + LoRa
- 5–10 puzzles based on environment + movement
- Manual thresholds for smoke/humidity/etc.

### v1.5 (Mid-Event OTA)
- TinyML-powered detection of smoke/rain
- Quest elements that react to detected environment type
- Optional: share AI predictions via LoRa to form a “weather mesh”

---

Let’s make this badge do something memorable. Hack on!

