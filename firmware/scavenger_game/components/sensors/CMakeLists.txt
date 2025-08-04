idf_component_register(
    SRCS "sensor_manager.c"
         "bme690_driver.c"
         "bmi270_driver.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)