#include "debug_manager.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "sensor_manager.h"
#include "quest_system.h"
#include "storage_manager.h"

static const char *TAG = "DEBUG_MANAGER";

static bool debug_logging_enabled = true;

esp_err_t debug_manager_init(void)
{
    ESP_LOGI(TAG, "Debug manager initialized");
    debug_logging_enabled = true;
    return ESP_OK;
}

void debug_manager_toggle_logging(void)
{
    debug_logging_enabled = !debug_logging_enabled;
    ESP_LOGI(TAG, "Debug logging %s", debug_logging_enabled ? "enabled" : "disabled");
}

bool debug_manager_is_logging_enabled(void)
{
    return debug_logging_enabled;
}

void debug_manager_log_sensor_data(void)
{
    if (!debug_logging_enabled) return;

    sensor_data_t data;
    if (sensor_manager_get_data(&data) == ESP_OK) {
        ESP_LOGI(TAG, "=== Sensor Data ===");
        ESP_LOGI(TAG, "Temperature: %.1f°C", data.temperature);
        ESP_LOGI(TAG, "Humidity: %.1f%%", data.humidity);
        ESP_LOGI(TAG, "Pressure: %.1f hPa", data.pressure);
        ESP_LOGI(TAG, "VOC: %lu", data.voc);
        ESP_LOGI(TAG, "Movement: %.2f", data.movement_magnitude);
        ESP_LOGI(TAG, "Tilt: %.1f°", data.tilt_angle);
        
        // Enhanced smoke detection display
        ESP_LOGI(TAG, "=== Air Quality ===");
        ESP_LOGI(TAG, "VOC Level: %lu", data.voc);
        if (data.voc < 350) {
            ESP_LOGI(TAG, "Air Quality: NORMAL");
        } else if (data.voc >= 350 && data.voc < 600) {
            ESP_LOGI(TAG, "Air Quality: CIGARETTE SMOKE DETECTED");
        } else if (data.voc >= 600) {
            ESP_LOGI(TAG, "Air Quality: CANNABIS SMOKE DETECTED");
        }
        
        // For data collection - output in parseable format
        ESP_LOGI(TAG, "VOC:%lu,TEMP:%.1f,HUM:%.1f", 
                 data.voc, data.temperature, data.humidity);
        
        ESP_LOGI(TAG, "=== Detection Status ===");
        ESP_LOGI(TAG, "Rain: %s", sensor_manager_is_rain_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Cold: %s", sensor_manager_is_cold_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Dark: %s", sensor_manager_is_dark_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Cigarette: %s", sensor_manager_is_cigarette_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Cannabis: %s", sensor_manager_is_cannabis_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Movement: %s", sensor_manager_is_movement_detected() ? "YES" : "NO");
        ESP_LOGI(TAG, "Tilt: %s", sensor_manager_is_tilt_detected() ? "YES" : "NO");
    }
}

void debug_manager_log_quest_state(void)
{
    if (!debug_logging_enabled) return;

    player_state_t state;
    if (quest_get_player_state(&state) == ESP_OK) {
        ESP_LOGI(TAG, "=== Quest State ===");
        ESP_LOGI(TAG, "Active quests: %d", state.active_quest_count);
        ESP_LOGI(TAG, "Completed quests: %d", state.completed_quest_count);
        ESP_LOGI(TAG, "Total score: %lu", state.total_score);
        
        for (int i = 0; i < state.active_quest_count; i++) {
            ESP_LOGI(TAG, "Quest %d: %s - %s (%lu/%lu)", 
                     state.quests[i].quest_id,
                     state.quests[i].name,
                     state.quests[i].status == QUEST_ACTIVE ? "ACTIVE" : 
                     state.quests[i].status == QUEST_COMPLETED ? "COMPLETED" : "INACTIVE",
                     state.quests[i].progress,
                     state.quests[i].target_value);
        }
    }
}

void debug_manager_print_memory_info(void)
{
    if (!debug_logging_enabled) return;

    ESP_LOGI(TAG, "=== Memory Info ===");
    ESP_LOGI(TAG, "Free heap: %zu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Min free heap: %zu bytes", esp_get_minimum_free_heap_size());
    
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "Total free: %zu bytes", info.total_free_bytes);
    ESP_LOGI(TAG, "Largest free block: %zu bytes", info.largest_free_block);
    ESP_LOGI(TAG, "Total allocated: %zu bytes", info.total_allocated_bytes);
}