#ifndef BMI270_DRIVER_H
#define BMI270_DRIVER_H

#include "esp_err.h"

esp_err_t bmi270_init(void);
esp_err_t bmi270_read_data(float *accel_x, float *accel_y, float *accel_z,
                          float *gyro_x, float *gyro_y, float *gyro_z);

#endif // BMI270_DRIVER_H