/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       IST8310.c/h
  * @brief      IST8310磁力计驱动函数，包括初始化函数，处理数据函数，通信读取函数
  *             本工程是将MPU6500 IIC_SLV0设置为自动读取IST8310数据，读取
  *             MPU_EXT_SENS_DATA_00保存了IST8310的Status，通过判断标志位，来更新
  *             数据。
  * @note       IST8310只支持IIC读取
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */

#include "ist8310driver.h"
#include "ist8310driver_middleware.h"
#include "math.h"

#define MAG_SEN 0.3f //转换成 uT

#define IST8310_WHO_AM_I 0x00       //ist8310 who am I 寄存器
#define IST8310_WHO_AM_I_VALUE 0x10 //设备 ID

#define IST8310_WRITE_REG_NUM 4 //IST8310需要设置的寄存器数目
#define IST8310_MAG_CAL_COUNT 1500

static const uint8_t ist8310_write_reg_data_error[IST8310_WRITE_REG_NUM][3] =
    {
        {0x0B, 0x08, 0x01},
        {0x41, 0x09, 0x02},
        {0x42, 0xC0, 0x03},
        {0x0A, 0x0B, 0x04}};

float mag_max[3] = {-32767.0f, -32767.0f, -32767.0f}, mag_min[3] = {32767.0f, 32767.0f, 32767.0f};


uint8_t ist8310_init(void)
{
    static const uint8_t wait_time = 1;
    static const uint8_t sleepTime = 50;
    uint8_t res = 0;
    uint8_t writeNum = 0;

    ist8310_GPIO_init();
    ist8310_com_init();

    ist8310_RST_L();
    ist8310_delay_ms(sleepTime);
    ist8310_RST_H();
    ist8310_delay_ms(sleepTime);

    res = ist8310_IIC_read_single_reg(IST8310_WHO_AM_I);
    if (res != IST8310_WHO_AM_I_VALUE)
    {
        return IST8310_NO_SENSOR;
    }
    ist8310_delay_ms(wait_time);
    //set mpu6500 sonsor config and check
    for (writeNum = 0; writeNum < IST8310_WRITE_REG_NUM; writeNum++)
    {
        ist8310_IIC_write_single_reg(ist8310_write_reg_data_error[writeNum][0], ist8310_write_reg_data_error[writeNum][1]);
        ist8310_delay_ms(wait_time);
        res = ist8310_IIC_read_single_reg(ist8310_write_reg_data_error[writeNum][0]);
        ist8310_delay_ms(wait_time);
        if (res != ist8310_write_reg_data_error[writeNum][1])
        {
            return ist8310_write_reg_data_error[writeNum][2];
        }
    }

    return IST8310_NO_ERROR;
}

void ist8310_read_over(uint8_t *status_buf, ist8310_real_data_t *ist8310_real_data)
{

    if (status_buf[0] & 0x01)
    {
        int16_t temp_ist8310_data = 0;
        ist8310_real_data->status |= 1 << IST8310_DATA_READY_BIT;

        temp_ist8310_data = (int16_t)((status_buf[2] << 8) | status_buf[1]);
        ist8310_real_data->mag[0] = MAG_SEN * temp_ist8310_data;
        temp_ist8310_data = (int16_t)((status_buf[4] << 8) | status_buf[3]);
        ist8310_real_data->mag[1] = MAG_SEN * temp_ist8310_data;
        temp_ist8310_data = (int16_t)((status_buf[6] << 8) | status_buf[5]);
        ist8310_real_data->mag[2] = MAG_SEN * temp_ist8310_data;
    }
    else
    {
        ist8310_real_data->status &= ~(1 << IST8310_DATA_READY_BIT);
    }
}

void ist8310_read_mag(fp32 mag[3])
{
    uint8_t buf[6];
    int16_t temp_ist8310_data = 0;
    ist8310_IIC_read_muli_reg(0x03, buf, 6);

    temp_ist8310_data = (int16_t)((buf[1] << 8) | buf[0]);
    mag[0] = MAG_SEN * -temp_ist8310_data;
    temp_ist8310_data = (int16_t)((buf[3] << 8) | buf[2]);
    mag[1] = MAG_SEN * temp_ist8310_data;
    temp_ist8310_data = (int16_t)((buf[5] << 8) | buf[4]);
    mag[2] = MAG_SEN * temp_ist8310_data; // 更改坐标系到bmi088
}

void ist8310_mag_collect(void)
{
    float mag_temp[3] = {0.0f, 0.0f, 0.0f};
    static uint8_t once_flag[3][2] = {0};
    ist8310_read_mag(mag_temp);
    
    for (int jj = 0; jj < 3; jj++) {
        if(once_flag[jj][0] == 0){
            once_flag[jj][0]=1;
        }
        else if(fabs(mag_temp[jj]-mag_max[jj]) > 1.0f){
            continue;
        }
        if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
    }
    for (int jj = 0; jj < 3; jj++) {
        if(once_flag[jj][1] == 0){
            once_flag[jj][1]=1;
        }
        else if(fabs(mag_temp[jj]-mag_min[jj]) > 1.0f){
            continue;
        }
        if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
    }
}

void ist8310_mag_cal(fp32 magBias[3], fp32 magScale[3])
{
    
    int32_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
    
    mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
    mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
    mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts
    
    magBias[0] = (float) mag_bias[0];  // save mag biases in G for main program
    magBias[1] = (float) mag_bias[1];   
    magBias[2] = (float) mag_bias[2];  
       
    // Get soft iron correction estimate
    mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
    mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
    mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

    float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
    avg_rad /= 3.0f;

    magScale[0] = avg_rad/((float)mag_scale[0]);
    magScale[1] = avg_rad/((float)mag_scale[1]);
    magScale[2] = avg_rad/((float)mag_scale[2]);
    
}
