#ifndef BME690_DRIVER_H
#define BME690_DRIVER_H

#include "esp_err.h"

esp_err_t bme690_init(void);
esp_err_t bme690_read_data(float *temperature, float *humidity, float *pressure, uint32_t *voc);

#endif // BME690_DRIVER_H