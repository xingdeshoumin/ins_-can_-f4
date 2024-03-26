#ifndef _FLASH_STORAGE_H_
#define _FLASH_STORAGE_H_

#include "main.h"

void Flash_SaveAngleCal(float cal_offset);
int Flash_ReadAngleCal(float* cal_offset);
void Flash_SaveMagCal(float magBias[3], float magScale[3]);
int Flash_ReadMagCal(float magBias[3], float magScale[3]);
void Flash_SaveMpuCal(float AccelScale, float GyroOffset[3]);
int Flash_ReadMpuCal(float *AccelScale, float GyroOffset[3]);

#endif
