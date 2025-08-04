# TinyML Training Guide for Smoke Detection

This guide explains how to collect training data and train a TinyML model to distinguish between different types of smoke using the WHY2025 badge's BME690 VOC sensor.

## Overview

The BME690 sensor measures Volatile Organic Compounds (VOCs) which have different signatures for:
- **Normal air**: VOC < 350
- **Cigarette smoke**: VOC 350-600 (sharp, chemical profile)
- **Herbal smoke**: VOC > 600 (complex organic compound profile)

## Data Collection Protocol

### 1. Setup

1. Flash the badge with the scavenger hunt firmware
2. Connect via USB serial (115200 baud)
3. Enable debug mode to see real-time VOC values

### 2. Collection Procedure

#### Normal Air Baseline
```bash
./tools/voc_data_collector.py /dev/ttyUSB0 --label normal --duration 60
```
- Collect in clean air environment
- Away from any smoke sources
- Multiple locations (indoor/outdoor)

#### Cigarette Smoke
```bash
./tools/voc_data_collector.py /dev/ttyUSB0 --label cigarette --duration 120
```
- Find designated smoking area
- Keep badge 1-2 meters from source
- Collect during active smoking
- **Safety**: Avoid direct smoke exposure

#### Herbal Smoke
```bash
./tools/voc_data_collector.py /dev/ttyUSB0 --label herbal --duration 120
```
- Similar distance as cigarette
- Note: Higher VOC values expected
- Collect in well-ventilated area
- **Legal**: Only where permitted

### 3. Interactive Collection

For more control, use interactive mode:
```bash
./tools/voc_data_collector.py /dev/ttyUSB0 --interactive
```

Commands:
- `collect normal 30` - Collect normal air for 30 seconds
- `collect cigarette 60` - Collect cigarette smoke for 60 seconds  
- `collect herbal 60` - Collect herbal smoke for 60 seconds
- `save training_data.csv` - Save all collected data
- `clear` - Clear buffer
- `quit` - Exit

## Data Analysis

### Visualize Collected Data
```bash
./tools/voc_data_visualizer.py training_data.csv
```

This shows:
- VOC distributions by label
- Time series plots
- Temperature/humidity correlations
- Statistical separability analysis

### Expected Patterns

1. **Cigarette smoke**:
   - Sharp VOC spike (350-500)
   - Quick rise and fall
   - Less complex chemical profile

2. **Herbal smoke**:
   - Higher VOC peak (600-800+)
   - Slower decay
   - More volatile terpenes

3. **Environmental factors**:
   - Temperature affects VOC readings
   - Humidity can amplify some compounds
   - Wind/ventilation affects concentration

## Model Training

### 1. Export Training Data

From the badge:
```c
// In debug mode, press 'E' to export
sensor_manager_export_voc_data("/sdcard/voc_training.csv");
```

### 2. Prepare Dataset

Combine multiple collection sessions:
```bash
cat voc_data_*.csv > combined_training.csv
```

### 3. Feature Engineering

The model uses:
- VOC value (primary)
- Temperature (context)
- Humidity (context)
- Rolling averages
- Rate of change

### 4. TinyML Model Architecture

Recommended approach:
- Small neural network (2-3 layers)
- Input: [VOC, temp, humidity, voc_delta]
- Hidden: 8-16 neurons
- Output: 3 classes (normal, cigarette, herbal)

### 5. Training Tips

1. **Balance classes**: Equal samples per category
2. **Cross-validation**: Test on different days/locations
3. **Quantization**: Convert to int8 for ESP32
4. **Size limit**: Keep model < 50KB

## Deployment

### 1. Convert Model
```python
# Convert to TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()
```

### 2. Generate C Array
```bash
xxd -i model.tflite > model_data.h
```

### 3. Update Firmware
The model will be deployed via OTA update after WHY2025.

## Testing

### Field Testing Protocol

1. **Baseline test**: Normal air in multiple environments
2. **Cigarette test**: Known cigarette smoke
3. **Herbal test**: Various herbal smoke sources
4. **Edge cases**: Incense, cooking smoke, vapor

### Performance Metrics

Target accuracy:
- Normal air: 95%+
- Cigarette: 85%+
- Herbal: 80%+

## Safety & Legal Notes

- **Health**: Avoid direct smoke inhalation
- **Legal**: Follow all local laws and regulations
- **Privacy**: Don't collect data without consent
- **Ethics**: Use for harm reduction, not surveillance

## Troubleshooting

### Low Accuracy
- Collect more diverse data
- Check sensor calibration
- Consider environmental factors

### False Positives
- Adjust detection thresholds
- Add time-based filtering
- Consider context (temp/humidity)

### Model Size Issues
- Reduce layers/neurons
- Increase quantization
- Use pruning techniques

## Next Steps

1. Collect initial dataset (Day 1 at WHY2025)
2. Train baseline model
3. Field test with other badge holders
4. Refine based on real-world data
5. Deploy via OTA update