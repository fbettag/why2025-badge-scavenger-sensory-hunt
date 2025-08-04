idf_component_register(
    SRCS "debug_manager.c"
         "logging_system.c"
    INCLUDE_DIRS "."
    REQUIRES log
)