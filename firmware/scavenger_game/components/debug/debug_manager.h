#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include "esp_err.h"

esp_err_t debug_manager_init(void);
void debug_manager_toggle_logging(void);
bool debug_manager_is_logging_enabled(void);
void debug_manager_log_sensor_data(void);
void debug_manager_log_quest_state(void);
void debug_manager_print_memory_info(void);

#endif // DEBUG_MANAGER_H