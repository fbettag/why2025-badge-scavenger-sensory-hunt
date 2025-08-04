idf_component_register(
    SRCS "quest_system.c"
         "quest_parser.c"
         "quest_conditions.c"
    INCLUDE_DIRS "."
    REQUIRES sensors storage debug
)