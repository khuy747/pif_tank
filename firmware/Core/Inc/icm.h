/*
 * icm.h
 *
 *  Created on: Nov 1, 2026
 *      Author: KinQuy
 */

#ifndef INC_ICM_H_
#define INC_ICM_H_

#include <stdio.h>
#include <main.h>

/* USER CODE BEGIN PV */

typedef struct
{
    int16_t ax_raw, ay_raw, az_raw;
    int16_t gx_raw, gy_raw, gz_raw;
    int16_t temp_raw;

    float ax_g, ay_g, az_g;
    float gx_dps, gy_dps, gz_dps;
    float temp_c;

    float gx_bias;
    float gy_bias;
    float gz_bias;

} ICM_20602_t;


HAL_StatusTypeDef icm_init(I2C_HandleTypeDef *I2Cx);

HAL_StatusTypeDef icm_readgyro(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu
);

HAL_StatusTypeDef icm_calibrate_gyro(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu,
    uint16_t samples
);

// Read all sensor data
HAL_StatusTypeDef icm_readall(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu
);
#endif /* INC_ICM_H_ */
