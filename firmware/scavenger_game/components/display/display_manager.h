#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "esp_err.h"

esp_err_t display_manager_init(void);
void display_manager_show_welcome(void);
void display_manager_show_quest_list(void);
void display_manager_show_quest_details(uint8_t quest_id);
void display_manager_show_sensor_data(void);
void display_manager_update_status(const char* status);

#endif // DISPLAY_MANAGER_H