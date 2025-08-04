#include "ml_model_manager.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include <string.h>

static const char *TAG = "ML_MODEL_MANAGER";

// Model state
typedef struct {
    bool initialized;
    bool loaded;
    uint8_t* model_data;
    size_t model_size;
    char model_version[32];
} ml_model_state_t;

static ml_model_state_t models[MODEL_TYPE_MAX] = {0};

esp_err_t ml_model_init(void)
{
    ESP_LOGI(TAG, "Initializing ML model manager");
    
    // Initialize model states
    for (int i = 0; i < MODEL_TYPE_MAX; i++) {
        models[i].initialized = true;
        models[i].loaded = false;
        models[i].model_data = NULL;
        models[i].model_size = 0;
        strcpy(models[i].model_version, "v1.0-placeholder");
    }
    
    ESP_LOGI(TAG, "ML model manager initialized");
    return ESP_OK;
}

esp_err_t ml_model_load(ml_model_type_t model_type, const char* model_path)
{
    if (model_type >= MODEL_TYPE_MAX || !model_path) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!models[model_type].initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "Loading model type %d from %s", model_type, model_path);
    
    // TODO: Implement actual model loading from SD card or flash
    // For now, mark as loaded with placeholder
    models[model_type].loaded = true;
    
    ESP_LOGI(TAG, "Model loaded successfully (placeholder)");
    return ESP_OK;
}

esp_err_t ml_model_inference(ml_model_type_t model_type, const void* input_data, ml_inference_result_t* result)
{
    if (model_type >= MODEL_TYPE_MAX || !input_data || !result) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!models[model_type].loaded) {
        ESP_LOGW(TAG, "Model not loaded, using threshold-based fallback");
        return ESP_ERR_INVALID_STATE;
    }
    
    // TODO: Implement actual TinyML inference
    // This is a placeholder that will be replaced with real model
    
    return ESP_OK;
}

esp_err_t ml_model_update(ml_model_type_t model_type, const uint8_t* model_data, size_t model_size)
{
    if (model_type >= MODEL_TYPE_MAX || !model_data || model_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Updating model type %d, size: %zu bytes", model_type, model_size);
    
    // Free old model if exists
    if (models[model_type].model_data) {
        heap_caps_free(models[model_type].model_data);
    }
    
    // Allocate new model in SPIRAM if available, otherwise in internal RAM
    models[model_type].model_data = heap_caps_malloc(model_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!models[model_type].model_data) {
        models[model_type].model_data = heap_caps_malloc(model_size, MALLOC_CAP_8BIT);
        if (!models[model_type].model_data) {
            ESP_LOGE(TAG, "Failed to allocate memory for model");
            return ESP_ERR_NO_MEM;
        }
    }
    
    memcpy(models[model_type].model_data, model_data, model_size);
    models[model_type].model_size = model_size;
    models[model_type].loaded = true;
    
    ESP_LOGI(TAG, "Model updated successfully");
    return ESP_OK;
}

esp_err_t ml_model_get_info(ml_model_type_t model_type, char* info_buffer, size_t buffer_size)
{
    if (model_type >= MODEL_TYPE_MAX || !info_buffer || buffer_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    snprintf(info_buffer, buffer_size, 
             "Model Type: %d\n"
             "Version: %s\n"
             "Loaded: %s\n"
             "Size: %zu bytes\n",
             model_type,
             models[model_type].model_version,
             models[model_type].loaded ? "Yes" : "No",
             models[model_type].model_size);
    
    return ESP_OK;
}

esp_err_t ml_voc_classify(uint32_t voc, float temp, float humidity, ml_inference_result_t* result)
{
    if (!result) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Store input values
    result->voc_value = voc;
    result->temperature = temp;
    result->humidity = humidity;
    
    // Check if model is loaded
    if (models[MODEL_VOC_CLASSIFIER].loaded) {
        // TODO: Run actual ML inference
        ESP_LOGD(TAG, "Running ML inference on VOC=%lu, T=%.1f, H=%.1f", voc, temp, humidity);
        
        // Placeholder: Will be replaced with actual model inference
        result->classification = VOC_CLASS_UNKNOWN;
        result->confidence = 0.0f;
        
        return ml_model_inference(MODEL_VOC_CLASSIFIER, &voc, result);
    } else {
        // Fallback to threshold-based classification
        ESP_LOGD(TAG, "Using threshold-based classification (no ML model)");
        
        if (voc < 350) {
            result->classification = VOC_CLASS_NORMAL;
            result->confidence = 0.9f;
        } else if (voc >= 350 && voc < 600) {
            result->classification = VOC_CLASS_CIGARETTE;
            result->confidence = 0.7f;
        } else if (voc >= 600) {
            result->classification = VOC_CLASS_HERBAL;
            result->confidence = 0.6f;
        } else {
            result->classification = VOC_CLASS_UNKNOWN;
            result->confidence = 0.0f;
        }
    }
    
    return ESP_OK;
}

const char* ml_voc_class_to_string(voc_class_t voc_class)
{
    switch (voc_class) {
        case VOC_CLASS_NORMAL:     return "Normal Air";
        case VOC_CLASS_CIGARETTE:  return "Cigarette Smoke";
        case VOC_CLASS_HERBAL:     return "Herbal Smoke";
        case VOC_CLASS_OTHER:      return "Other Smoke";
        case VOC_CLASS_UNKNOWN:    return "Unknown";
        default:                   return "Invalid";
    }
}