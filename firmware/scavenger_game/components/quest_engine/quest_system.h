#ifndef QUEST_SYSTEM_H
#define QUEST_SYSTEM_H

#include "stdint.h"
#include "stdbool.h"
#include "esp_err.h"

#define MAX_QUESTS 20
#define MAX_QUEST_NAME_LEN 32
#define MAX_QUEST_DESC_LEN 128
#define MAX_QUESTS_PER_PLAYER 10

typedef enum {
    QUEST_INACTIVE = 0,
    QUEST_ACTIVE,
    QUEST_COMPLETED,
    QUEST_FAILED
} quest_status_t;

typedef enum {
    TRIGGER_NONE = 0,
    TRIGGER_RAIN,
    TRIGGER_COLD,
    TRIGGER_DARK,
    TRIGGER_SMOKE,        // Cigarette smoke
    TRIGGER_HERBAL,       // Herbal smoke (ML-enhanced)
    TRIGGER_MOVEMENT,
    TRIGGER_TILT,
    TRIGGER_PROXIMITY,
    TRIGGER_MANUAL
} trigger_type_t;

typedef struct {
    uint8_t quest_id;
    char name[MAX_QUEST_NAME_LEN];
    char description[MAX_QUEST_DESC_LEN];
    trigger_type_t trigger_type;
    uint32_t trigger_threshold;
    quest_status_t status;
    uint32_t progress;
    uint32_t target_value;
    uint32_t completed_timestamp;
} quest_t;

typedef struct {
    uint8_t active_quest_count;
    uint8_t completed_quest_count;
    uint32_t total_score;
    quest_t quests[MAX_QUESTS_PER_PLAYER];
} player_state_t;

esp_err_t quest_system_init(void);
void quest_system_update(void);
esp_err_t quest_add(const char* name, const char* description, trigger_type_t trigger, uint32_t target);
esp_err_t quest_activate(uint8_t quest_id);
esp_err_t quest_complete(uint8_t quest_id);
esp_err_t quest_get_state(uint8_t quest_id, quest_t* quest);
esp_err_t quest_get_player_state(player_state_t* state);
bool quest_check_trigger(trigger_type_t trigger);

#endif // QUEST_SYSTEM_H