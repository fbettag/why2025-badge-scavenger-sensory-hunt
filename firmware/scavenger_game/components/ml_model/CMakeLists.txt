idf_component_register(
    SRCS "ml_model_manager.c"
         "voc_classifier.c"
    INCLUDE_DIRS "."
    REQUIRES sensors storage debug
)