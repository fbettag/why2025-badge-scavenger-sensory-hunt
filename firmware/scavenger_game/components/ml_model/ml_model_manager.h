#ifndef ML_MODEL_MANAGER_H
#define ML_MODEL_MANAGER_H

#include "stdint.h"
#include "esp_err.h"

// Model types
typedef enum {
    MODEL_VOC_CLASSIFIER = 0,
    MODEL_TYPE_MAX
} ml_model_type_t;

// VOC classification results
typedef enum {
    VOC_CLASS_NORMAL = 0,
    VOC_CLASS_CIGARETTE,
    VOC_CLASS_HERBAL,
    VOC_CLASS_OTHER,
    VOC_CLASS_UNKNOWN
} voc_class_t;

typedef struct {
    float confidence;
    voc_class_t classification;
    uint32_t voc_value;
    float temperature;
    float humidity;
} ml_inference_result_t;

// Initialize ML model manager
esp_err_t ml_model_init(void);

// Load model from storage
esp_err_t ml_model_load(ml_model_type_t model_type, const char* model_path);

// Run inference
esp_err_t ml_model_inference(ml_model_type_t model_type, const void* input_data, ml_inference_result_t* result);

// Update model (for future OTA updates)
esp_err_t ml_model_update(ml_model_type_t model_type, const uint8_t* model_data, size_t model_size);

// Get model info
esp_err_t ml_model_get_info(ml_model_type_t model_type, char* info_buffer, size_t buffer_size);

// VOC-specific functions
esp_err_t ml_voc_classify(uint32_t voc, float temp, float humidity, ml_inference_result_t* result);
const char* ml_voc_class_to_string(voc_class_t voc_class);

#endif // ML_MODEL_MANAGER_H