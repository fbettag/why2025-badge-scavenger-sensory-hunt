#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "stdint.h"
#include "esp_err.h"

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint32_t voc;
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float tilt_angle;
    float movement_magnitude;
} sensor_data_t;

esp_err_t sensor_manager_init(void);
esp_err_t sensor_manager_get_data(sensor_data_t *data);
bool sensor_manager_is_rain_detected(void);
bool sensor_manager_is_cold_detected(void);
bool sensor_manager_is_dark_detected(void);
bool sensor_manager_is_smoke_detected(void);  // Deprecated - use cigarette_detected
bool sensor_manager_is_cigarette_detected(void);
bool sensor_manager_is_herbal_detected(void);
bool sensor_manager_is_movement_detected(void);
bool sensor_manager_is_tilt_detected(void);

// Data collection for ML training
void sensor_manager_start_voc_logging(const char* label);
void sensor_manager_stop_voc_logging(void);
esp_err_t sensor_manager_export_voc_data(const char* filename);

#endif // SENSOR_MANAGER_H