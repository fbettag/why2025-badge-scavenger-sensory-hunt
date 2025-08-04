idf_component_register(
    SRCS "storage_manager.c"
         "state_persistence.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash fatfs spi_flash
)