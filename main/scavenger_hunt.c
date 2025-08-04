#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "quest_system.h"
#include "sensor_manager.h"
#include "display_manager.h"
#include "lora_manager.h"
#include "storage_manager.h"
#include "debug_manager.h"

static const char *TAG = "SCAVENGER_SENSORY_HUNT";

void app_main(void)
{
    ESP_LOGI(TAG, "=== WHY2025 Scavenger Sensory Hunt Starting ===");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize system components
    debug_manager_init();
    storage_manager_init();
    sensor_manager_init();
    display_manager_init();
    lora_manager_init();
    quest_system_init();

    ESP_LOGI(TAG, "All systems initialized successfully");
    ESP_LOGI(TAG, "Starting main game loop...");

    while (1) {
        // Main game loop
        quest_system_update();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}