#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include "esp_err.h"

esp_err_t lora_manager_init(void);
esp_err_t lora_manager_send_message(const char* message);
esp_err_t lora_manager_broadcast_presence(void);
bool lora_manager_is_nearby_badge_detected(void);
void lora_manager_update(void);

#endif // LORA_MANAGER_H