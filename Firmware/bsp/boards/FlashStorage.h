#ifndef _FLASH_STORAGE_H_
#define _FLASH_STORAGE_H_

#include "main.h"

void Flash_SaveMagCal(float magBias[3], float magScale[3]);
int Flash_ReadMagCal(float magBias[3], float magScale[3]);

#endif
