idf_component_register(
    SRCS "scavenger_hunt.c"
         "quest_system.c"
         "game_state.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES sensors quest_engine display lora storage debug
                  driver nvs_flash fatfs spi_flash freertos
)