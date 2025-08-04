#include "quest_system.h"
#include "sensor_manager.h"
#include "storage_manager.h"
#include "lora_manager.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "QUEST_SYSTEM";

static quest_t available_quests[MAX_QUESTS];
static player_state_t player_state;
static bool system_initialized = false;

// Quest definitions based on sensor triggers
static const quest_t default_quests[] = {
    {
        .quest_id = 1,
        .name = "Rain Dancer",
        .description = "Find a rainy spot and dance in the rain!",
        .trigger_type = TRIGGER_RAIN,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 2,
        .name = "Cold Explorer",
        .description = "Find a cold location (below 15°C)",
        .trigger_type = TRIGGER_COLD,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 3,
        .name = "Shadow Hunter",
        .description = "Find a dark or covered area",
        .trigger_type = TRIGGER_DARK,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 4,
        .name = "Smoke Detective",
        .description = "Detect cigarette smoke",
        .trigger_type = TRIGGER_SMOKE,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 9,
        .name = "Herbal Detective",
        .description = "Detect unique herbal smoke signatures",
        .trigger_type = TRIGGER_HERBAL,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 5,
        .name = "Shake It Off",
        .description = "Shake your badge vigorously",
        .trigger_type = TRIGGER_MOVEMENT,
        .trigger_threshold = 5,
        .target_value = 5,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 6,
        .name = "Tilt Master",
        .description = "Tilt your badge at different angles",
        .trigger_type = TRIGGER_TILT,
        .trigger_threshold = 3,
        .target_value = 3,
        .status = QUEST_INACTIVE
    },
    {
        .quest_id = 7,
        .name = "Badge Network",
        .description = "Find another badge nearby via LoRa",
        .trigger_type = TRIGGER_PROXIMITY,
        .trigger_threshold = 1,
        .target_value = 1,
        .status = QUEST_INACTIVE
    }
};

esp_err_t quest_system_init(void)
{
    if (system_initialized) {
        return ESP_OK;
    }

    // Initialize player state
    memset(&player_state, 0, sizeof(player_state));
    
    // Load available quests
    memcpy(available_quests, default_quests, sizeof(default_quests));
    
    // Load saved player state from storage
    storage_manager_load_player_state(&player_state);
    
    system_initialized = true;
    ESP_LOGI(TAG, "Quest system initialized with %d available quests", 
             sizeof(default_quests)/sizeof(default_quests[0]));
    
    return ESP_OK;
}

void quest_system_update(void)
{
    if (!system_initialized) {
        return;
    }

    // Check all active quests for trigger conditions
    for (int i = 0; i < player_state.active_quest_count && i < MAX_QUESTS_PER_PLAYER; i++) {
        quest_t *quest = &player_state.quests[i];
        
        if (quest->status != QUEST_ACTIVE) {
            continue;
        }
        
        if (quest_check_trigger(quest->trigger_type)) {
            quest->progress++;
            ESP_LOGD(TAG, "Quest '%s' progress: %lu/%lu", 
                     quest->name, quest->progress, quest->target_value);
            
            if (quest->progress >= quest->target_value) {
                quest_complete(quest->quest_id);
            }
        }
    }
}

esp_err_t quest_add(const char* name, const char* description, trigger_type_t trigger, uint32_t target)
{
    if (!name || !description || target == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    for (int i = 0; i < MAX_QUESTS; i++) {
        if (available_quests[i].status == QUEST_INACTIVE) {
            available_quests[i].quest_id = i + 1;
            available_quests[i].name[0] = '\0';
            available_quests[i].description[0] = '\0';
            strncat(available_quests[i].name, name, MAX_QUEST_NAME_LEN - 1);
            strncat(available_quests[i].description, description, MAX_QUEST_DESC_LEN - 1);
            available_quests[i].trigger_type = trigger;
            available_quests[i].target_value = target;
            available_quests[i].status = QUEST_INACTIVE;
            
            ESP_LOGI(TAG, "Added quest: %s", name);
            return ESP_OK;
        }
    }
    
    return ESP_ERR_NO_MEM;
}

esp_err_t quest_activate(uint8_t quest_id)
{
    if (quest_id == 0 || quest_id > MAX_QUESTS) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (player_state.active_quest_count >= MAX_QUESTS_PER_PLAYER) {
        return ESP_ERR_NO_MEM;
    }
    
    quest_t *quest = &available_quests[quest_id - 1];
    if (quest->status != QUEST_INACTIVE) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Add to player active quests
    player_state.quests[player_state.active_quest_count] = *quest;
    player_state.quests[player_state.active_quest_count].status = QUEST_ACTIVE;
    player_state.active_quest_count++;
    
    ESP_LOGI(TAG, "Activated quest: %s", quest->name);
    
    // Save state
    storage_manager_save_player_state(&player_state);
    
    return ESP_OK;
}

esp_err_t quest_complete(uint8_t quest_id)
{
    for (int i = 0; i < player_state.active_quest_count; i++) {
        if (player_state.quests[i].quest_id == quest_id) {
            quest_t *quest = &player_state.quests[i];
            quest->status = QUEST_COMPLETED;
            quest->completed_timestamp = esp_timer_get_time() / 1000000;
            player_state.completed_quest_count++;
            player_state.total_score += 100; // Base score per quest
            
            ESP_LOGI(TAG, "Quest completed: %s", quest->name);
            
            // Save state
            storage_manager_save_player_state(&player_state);
            
            return ESP_OK;
        }
    }
    
    return ESP_ERR_NOT_FOUND;
}

esp_err_t quest_get_state(uint8_t quest_id, quest_t* quest)
{
    if (!quest) {
        return ESP_ERR_INVALID_ARG;
    }
    
    for (int i = 0; i < player_state.active_quest_count; i++) {
        if (player_state.quests[i].quest_id == quest_id) {
            *quest = player_state.quests[i];
            return ESP_OK;
        }
    }
    
    return ESP_ERR_NOT_FOUND;
}

esp_err_t quest_get_player_state(player_state_t* state)
{
    if (!state) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *state = player_state;
    return ESP_OK;
}

bool quest_check_trigger(trigger_type_t trigger)
{
    switch (trigger) {
        case TRIGGER_RAIN:
            return sensor_manager_is_rain_detected();
        case TRIGGER_COLD:
            return sensor_manager_is_cold_detected();
        case TRIGGER_DARK:
            return sensor_manager_is_dark_detected();
        case TRIGGER_SMOKE:
            return sensor_manager_is_cigarette_detected();
        case TRIGGER_HERBAL:
            return sensor_manager_is_herbal_detected();
        case TRIGGER_MOVEMENT:
            return sensor_manager_is_movement_detected();
        case TRIGGER_TILT:
            return sensor_manager_is_tilt_detected();
        case TRIGGER_PROXIMITY:
            // TODO: Implement LoRa proximity detection
            return false;
        case TRIGGER_MANUAL:
            return false; // Manual triggers handled separately
        default:
            return false;
    }
}