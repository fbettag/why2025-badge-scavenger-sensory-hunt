#include "sensor_manager.h"
#include "bme690_driver.h"
#include "bmi270_driver.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <string.h>

static const char *TAG = "SENSOR_MANAGER";

static sensor_data_t current_data = {0};
static esp_timer_handle_t sensor_timer = NULL;
static bool initialized = false;

// Thresholds for environmental triggers
#define RAIN_HUMIDITY_THRESHOLD     85.0f
#define COLD_TEMP_THRESHOLD         15.0f
#define CIGARETTE_VOC_THRESHOLD     350   // Cigarette smoke
#define HERBAL_VOC_THRESHOLD        600   // Herbal smoke (will be ML-enhanced)
#define SMOKE_VOC_THRESHOLD         400   // Legacy threshold
#define MOVEMENT_THRESHOLD          1.5f
#define TILT_THRESHOLD              30.0f

// Data logging for ML
#define MAX_VOC_SAMPLES             1000
typedef struct {
    uint32_t timestamp;
    uint32_t voc_value;
    float temperature;
    float humidity;
    char label[32];
} voc_sample_t;

static voc_sample_t voc_samples[MAX_VOC_SAMPLES];
static uint32_t sample_count = 0;
static bool logging_enabled = false;
static char current_label[32] = {0};

static void sensor_timer_callback(void* arg)
{
    (void)arg;
    
    // Read BME690 data
    if (bme690_read_data(&current_data.temperature, &current_data.humidity, &current_data.pressure, &current_data.voc) == ESP_OK) {
        ESP_LOGD(TAG, "BME690: T=%.1f°C, H=%.1f%%, P=%.1f hPa, VOC=%lu", 
                 current_data.temperature, current_data.humidity, current_data.pressure, current_data.voc);
        
        // Log VOC data if enabled
        if (logging_enabled && sample_count < MAX_VOC_SAMPLES) {
            voc_samples[sample_count].timestamp = esp_timer_get_time() / 1000; // ms
            voc_samples[sample_count].voc_value = current_data.voc;
            voc_samples[sample_count].temperature = current_data.temperature;
            voc_samples[sample_count].humidity = current_data.humidity;
            voc_samples[sample_count].label[0] = '\0';
            strncat(voc_samples[sample_count].label, current_label, sizeof(voc_samples[sample_count].label) - 1);
            sample_count++;
        }
    }
    
    // Read BMI270 data
    if (bmi270_read_data(&current_data.accel_x, &current_data.accel_y, &current_data.accel_z,
                        &current_data.gyro_x, &current_data.gyro_y, &current_data.gyro_z) == ESP_OK) {
        
        // Calculate movement magnitude
        current_data.movement_magnitude = sqrt(
            current_data.accel_x * current_data.accel_x +
            current_data.accel_y * current_data.accel_y +
            current_data.accel_z * current_data.accel_z
        );
        
        // Calculate tilt angle (simplified)
        current_data.tilt_angle = atan2(current_data.accel_y, current_data.accel_z) * 180.0f / 3.14159f;
        
        ESP_LOGD(TAG, "BMI270: Movement=%.2f, Tilt=%.1f°", 
                 current_data.movement_magnitude, current_data.tilt_angle);
    }
}

esp_err_t sensor_manager_init(void)
{
    if (initialized) {
        return ESP_OK;
    }
    
    esp_err_t ret = ESP_OK;
    
    // Initialize BME690
    ret = bme690_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BME690");
        return ret;
    }
    
    // Initialize BMI270
    ret = bmi270_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BMI270");
        return ret;
    }
    
    // Create timer for periodic sensor reading
    esp_timer_create_args_t timer_args = {
        .callback = &sensor_timer_callback,
        .name = "sensor_timer"
    };
    
    ret = esp_timer_create(&timer_args, &sensor_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create sensor timer");
        return ret;
    }
    
    // Start timer (100ms interval)
    ret = esp_timer_start_periodic(sensor_timer, 100000);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start sensor timer");
        return ret;
    }
    
    initialized = true;
    ESP_LOGI(TAG, "Sensor manager initialized");
    return ESP_OK;
}

esp_err_t sensor_manager_get_data(sensor_data_t *data)
{
    if (!initialized || !data) {
        return ESP_ERR_INVALID_STATE;
    }
    
    *data = current_data;
    return ESP_OK;
}

bool sensor_manager_is_rain_detected(void)
{
    return current_data.humidity > RAIN_HUMIDITY_THRESHOLD;
}

bool sensor_manager_is_cold_detected(void)
{
    return current_data.temperature < COLD_TEMP_THRESHOLD;
}

bool sensor_manager_is_dark_detected(void)
{
    // Proxy detection: temp drop + humidity rise
    static float prev_temp = 0.0f;
    static float prev_humidity = 0.0f;
    static bool first_reading = true;
    
    if (first_reading) {
        prev_temp = current_data.temperature;
        prev_humidity = current_data.humidity;
        first_reading = false;
        return false;
    }
    
    float temp_drop = prev_temp - current_data.temperature;
    float humidity_rise = current_data.humidity - prev_humidity;
    
    prev_temp = current_data.temperature;
    prev_humidity = current_data.humidity;
    
    return (temp_drop > 2.0f) && (humidity_rise > 5.0f);
}

bool sensor_manager_is_smoke_detected(void)
{
    // Legacy function for compatibility
    return sensor_manager_is_cigarette_detected();
}

bool sensor_manager_is_cigarette_detected(void)
{
    return current_data.voc > CIGARETTE_VOC_THRESHOLD && current_data.voc < HERBAL_VOC_THRESHOLD;
}

bool sensor_manager_is_herbal_detected(void)
{
    // Basic threshold detection for now
    // Will be enhanced with ML model later
    return current_data.voc > HERBAL_VOC_THRESHOLD;
}

bool sensor_manager_is_movement_detected(void)
{
    return current_data.movement_magnitude > MOVEMENT_THRESHOLD;
}

bool sensor_manager_is_tilt_detected(void)
{
    return fabs(current_data.tilt_angle) > TILT_THRESHOLD;
}

// Data collection functions for ML training
void sensor_manager_start_voc_logging(const char* label)
{
    if (!initialized || !label) return;
    
    logging_enabled = true;
    current_label[0] = '\0';
    strncat(current_label, label, sizeof(current_label) - 1);
    
    ESP_LOGI(TAG, "Started VOC logging with label: %s", current_label);
}

void sensor_manager_stop_voc_logging(void)
{
    logging_enabled = false;
    ESP_LOGI(TAG, "Stopped VOC logging. Collected %lu samples", sample_count);
}

esp_err_t sensor_manager_export_voc_data(const char* filename)
{
    if (!filename || sample_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // TODO: Implement actual file export to SD card
    ESP_LOGI(TAG, "Exporting %lu VOC samples to %s", sample_count, filename);
    
    // For now, log the data format
    ESP_LOGI(TAG, "CSV Format: timestamp,voc,temperature,humidity,label");
    
    // Reset sample buffer after export
    sample_count = 0;
    
    return ESP_OK;
}