
#include "icm.h"

// I2C Address (SA0 = GND)
#define ICM_ADDR          (0x68U << 1)

// Register Addresses
#define ICM_WHO_AM_I      0x75
#define ICM_PWR_MGMT_1    0x6B
#define ICM_PWR_MGMT_2    0x6C

#define ICM_GYRO_CONFIG   0x1B
#define ICM_ACCEL_CONFIG  0x1C
#define ICM_ACCEL_CFG2    0x1D
#define ICM_CONFIG        0x1A
#define ICM_SMPLRT_DIV    0x19

#define ICM_ACCEL_XOUT_H  0x3B
#define ICM_GYRO_XOUT_H 0x43

#define ICM_TIMEOUT       100U

// ------------------------------------
// Write 1 byte to a register
// ------------------------------------
static HAL_StatusTypeDef icm_write(
    I2C_HandleTypeDef *I2Cx,
    uint8_t reg,
    uint8_t value)
{
    return HAL_I2C_Mem_Write(
        I2Cx,
        ICM_ADDR,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &value,
        1,
        ICM_TIMEOUT
    );
}

// ------------------------------------
// Read multiple bytes from a register
// ------------------------------------
static HAL_StatusTypeDef icm_read(
    I2C_HandleTypeDef *I2Cx,
    uint8_t reg,
    uint8_t *buffer,
    uint16_t length)
{
    return HAL_I2C_Mem_Read(
        I2Cx,
        ICM_ADDR,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        buffer,
        length,
        ICM_TIMEOUT
    );
}

// ------------------------------------
// Initialize ICM-20602
// ------------------------------------
HAL_StatusTypeDef icm_init(I2C_HandleTypeDef *I2Cx)
{
    uint8_t id = 0;
    uint8_t value = 0;
    HAL_StatusTypeDef status = HAL_OK;

    if (I2Cx == NULL)
        return HAL_ERROR;

    // 1. Check device response
    status = HAL_I2C_IsDeviceReady(
        I2Cx, ICM_ADDR, 3, ICM_TIMEOUT
    );

    if (status != HAL_OK)
        return status;

    // 2. Read WHO_AM_I
    status = icm_read(
        I2Cx, ICM_WHO_AM_I, &id, 1
    );

    if (status != HAL_OK)
        return status;

    // Expected ICM-20602 ID = 0x12
    if (id != 0x12)
        return HAL_ERROR;

    // 3. Reset device
    value = 0x80;

    status = icm_write(
        I2Cx, ICM_PWR_MGMT_1, value
    );

    if (status != HAL_OK)
        return status;

    HAL_Delay(100);

    // 4. Wake up and select clock
    value = 0x01;

    status = icm_write(
        I2Cx, ICM_PWR_MGMT_1, value
    );

    if (status != HAL_OK)
        return status;

    HAL_Delay(50);

    // 5. Enable all accel and gyro axes
    value = 0x00;

    status = icm_write(
        I2Cx, ICM_PWR_MGMT_2, value
    );

    if (status != HAL_OK)
        return status;

    // 6. Gyroscope: +/-250 deg/s
    value = 0x00;

    status = icm_write(
        I2Cx, ICM_GYRO_CONFIG, value
    );

    if (status != HAL_OK)
        return status;

    // 7. Accelerometer: +/-2g
    value = 0x00;

    status = icm_write(
        I2Cx, ICM_ACCEL_CONFIG, value
    );

    if (status != HAL_OK)
        return status;

    // 8. Gyroscope DLPF: approximately 41 Hz
    value = 0x03;

    status = icm_write(
        I2Cx, ICM_CONFIG, value
    );

    if (status != HAL_OK)
        return status;

    // 9. Accelerometer DLPF: approximately 44.8 Hz
    value = 0x03;

    status = icm_write(
        I2Cx, ICM_ACCEL_CFG2, value
    );

    if (status != HAL_OK)
        return status;

    // 10. Sample rate: 1000 / (1 + 9) = 100 Hz
    value = 9;

    status = icm_write(
        I2Cx, ICM_SMPLRT_DIV, value
    );

    if (status != HAL_OK)
        return status;

    HAL_Delay(100);

    return HAL_OK;
}


//READ Only Gyro


HAL_StatusTypeDef icm_readgyro(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu)
{
    if (I2Cx == NULL || imu == NULL)
        return HAL_ERROR;

    uint8_t buffer[6] = {0};

    HAL_StatusTypeDef status;

    // Read 6 bytes starting from GYRO_XOUT_H
    status = icm_read(
        I2Cx,
        ICM_GYRO_XOUT_H,
        buffer,
        6
    );

    if (status != HAL_OK)
        return status;

    // Combine high and low bytes
    imu->gx_raw = (int16_t)(
        ((uint16_t)buffer[0] << 8) | buffer[1]);

    imu->gy_raw = (int16_t)(
        ((uint16_t)buffer[2] << 8) | buffer[3]);

    imu->gz_raw = (int16_t)(
        ((uint16_t)buffer[4] << 8) | buffer[5]);

    // Convert to deg/s and subtract bias
    imu->gx_dps = imu->gx_raw / 131.0f - imu->gx_bias;
    imu->gy_dps = imu->gy_raw / 131.0f - imu->gy_bias;
    imu->gz_dps = imu->gz_raw / 131.0f - imu->gz_bias;

    return HAL_OK;
}


HAL_StatusTypeDef icm_calibrate_gyro(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu,
    uint16_t samples)
{
    if (I2Cx == NULL || imu == NULL || samples == 0)
        return HAL_ERROR;

    int32_t sum_x = 0;
    int32_t sum_y = 0;
    int32_t sum_z = 0;

    HAL_StatusTypeDef status;

    // Wait for sensor to stabilize
    HAL_Delay(150);

    for (uint16_t i = 0; i < samples; i++)
    {
        status = icm_readgyro(I2Cx, imu);

        if (status != HAL_OK)
            return status;

        // Accumulate uncorrected RAW readings
        sum_x += imu->gx_raw;
        sum_y += imu->gy_raw;
        sum_z += imu->gz_raw;

        HAL_Delay(10);
    }

    // Calculate average bias in deg/s
    imu->gx_bias = ((float)sum_x / samples) / 131.0f;
    imu->gy_bias = ((float)sum_y / samples) / 131.0f;
    imu->gz_bias = ((float)sum_z / samples) / 131.0f;

    return HAL_OK;
}


// ------------------------------------
// Read Accel, Gyro and Temperature
// ------------------------------------
HAL_StatusTypeDef icm_readall(
    I2C_HandleTypeDef *I2Cx,
    ICM_20602_t *imu)
{
    if (I2Cx == NULL || imu == NULL)
        return HAL_ERROR;

    uint8_t buffer[14] = {0};
    HAL_StatusTypeDef status;

    // Read 14 consecutive bytes from 0x3B
    status = icm_read(
        I2Cx,
        ICM_ACCEL_XOUT_H,
        buffer,
        14
    );

    if (status != HAL_OK)
        return status;

    // Raw accelerometer
    imu->ax_raw = (int16_t)(
        ((uint16_t)buffer[0] << 8) | buffer[1]);

    imu->ay_raw = (int16_t)(
        ((uint16_t)buffer[2] << 8) | buffer[3]);

    imu->az_raw = (int16_t)(
        ((uint16_t)buffer[4] << 8) | buffer[5]);

    // Raw temperature
    imu->temp_raw = (int16_t)(
        ((uint16_t)buffer[6] << 8) | buffer[7]);

    // Raw gyroscope
    imu->gx_raw = (int16_t)(
        ((uint16_t)buffer[8] << 8) | buffer[9]);

    imu->gy_raw = (int16_t)(
        ((uint16_t)buffer[10] << 8) | buffer[11]);

    imu->gz_raw = (int16_t)(
        ((uint16_t)buffer[12] << 8) | buffer[13]);

    // Convert accelerometer to g
    imu->ax_g = imu->ax_raw / 16384.0f;
    imu->ay_g = imu->ay_raw / 16384.0f;
    imu->az_g = imu->az_raw / 16384.0f;

    // Convert gyroscope to deg/s
    imu->gx_dps = imu->gx_raw / 131.0f;
    imu->gy_dps = imu->gy_raw / 131.0f;
    imu->gz_dps = imu->gz_raw / 131.0f;

    // Convert temperature to Celsius
    imu->temp_c = imu->temp_raw / 326.8f + 25.0f;

    return HAL_OK;
}
