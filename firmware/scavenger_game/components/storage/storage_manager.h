#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "stdint.h"
#include "esp_err.h"
#include "quest_system.h"

esp_err_t storage_manager_init(void);
esp_err_t storage_manager_save_player_state(const player_state_t* state);
esp_err_t storage_manager_load_player_state(player_state_t* state);
esp_err_t storage_manager_save_quest_data(const void* data, size_t length);
esp_err_t storage_manager_load_quest_data(void* data, size_t length);
esp_err_t storage_manager_clear_all_data(void);

// SD card functions
esp_err_t storage_manager_mount_sd(void);
esp_err_t storage_manager_unmount_sd(void);

#endif // STORAGE_MANAGER_H