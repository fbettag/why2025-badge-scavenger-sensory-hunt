idf_component_register(
    SRCS "lora_manager.c"
         "lora_communication.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)