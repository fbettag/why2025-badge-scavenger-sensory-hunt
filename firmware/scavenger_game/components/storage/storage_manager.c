#include "storage_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

static const char *TAG = "STORAGE_MANAGER";

static nvs_handle_t nvs_handle;
static bool nvs_initialized = false;
static sdmmc_card_t* card = NULL;
static bool sd_mounted = false;

#define STORAGE_NAMESPACE "scavenger_sensory_hunt"
#define PLAYER_STATE_KEY "player_state"
#define QUEST_DATA_KEY "quest_data"

esp_err_t storage_manager_init(void)
{
    if (nvs_initialized) {
        return ESP_OK;
    }

    // Initialize NVS
    esp_err_t ret = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS handle: %s", esp_err_to_name(ret));
        return ret;
    }

    nvs_initialized = true;
    ESP_LOGI(TAG, "Storage manager initialized");
    return ESP_OK;
}

esp_err_t storage_manager_save_player_state(const player_state_t* state)
{
    if (!nvs_initialized || !state) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = nvs_set_blob(nvs_handle, PLAYER_STATE_KEY, state, sizeof(player_state_t));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save player state: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit player state: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGD(TAG, "Player state saved successfully");
    return ESP_OK;
}

esp_err_t storage_manager_load_player_state(player_state_t* state)
{
    if (!nvs_initialized || !state) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t required_size = sizeof(player_state_t);
    esp_err_t ret = nvs_get_blob(nvs_handle, PLAYER_STATE_KEY, state, &required_size);
    
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        // Initialize with default values
        memset(state, 0, sizeof(player_state_t));
        ESP_LOGI(TAG, "No saved player state found, initializing with defaults");
        return ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load player state: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGD(TAG, "Player state loaded successfully");
    return ESP_OK;
}

esp_err_t storage_manager_save_quest_data(const void* data, size_t length)
{
    if (!nvs_initialized || !data || length == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = nvs_set_blob(nvs_handle, QUEST_DATA_KEY, data, length);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save quest data: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit quest data: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGD(TAG, "Quest data saved successfully (%zu bytes)", length);
    return ESP_OK;
}

esp_err_t storage_manager_load_quest_data(void* data, size_t length)
{
    if (!nvs_initialized || !data || length == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t required_size = length;
    esp_err_t ret = nvs_get_blob(nvs_handle, QUEST_DATA_KEY, data, &required_size);
    
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No saved quest data found");
        return ESP_ERR_NOT_FOUND;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load quest data: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGD(TAG, "Quest data loaded successfully (%zu bytes)", required_size);
    return ESP_OK;
}

esp_err_t storage_manager_clear_all_data(void)
{
    if (!nvs_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = nvs_erase_all(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear all data: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit clear operation: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "All storage data cleared");
    return ESP_OK;
}

esp_err_t storage_manager_mount_sd(void)
{
    if (sd_mounted) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Mounting SD card...");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width to 4-bit
    host.flags = SDMMC_HOST_FLAG_4BIT;
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

    // GPIOs for 4-bit mode
    slot_config.width = 4;
    slot_config.clk = 14;
    slot_config.cmd = 15;
    slot_config.d0 = 2;
    slot_config.d1 = 4;
    slot_config.d2 = 12;
    slot_config.d3 = 13;

    // Mount FAT filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
    sd_mounted = true;

    return ESP_OK;
}

esp_err_t storage_manager_unmount_sd(void)
{
    if (!sd_mounted) {
        return ESP_OK;
    }

    esp_err_t ret = esp_vfs_fat_sdmmc_unmount("/sdcard", card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount SD card: %s", esp_err_to_name(ret));
        return ret;
    }

    sd_mounted = false;
    card = NULL;
    ESP_LOGI(TAG, "SD card unmounted");
    return ESP_OK;
}