#include "FlashStorage.h"
#include <string.h>
#include "bsp_flash.h"

#define FLAH_STORAGE_ANG_ADDR ADDR_FLASH_SECTOR_5
#define FLAH_STORAGE_CAL_ADDR ADDR_FLASH_SECTOR_4
#define FLAH_STORAGE_MPU_ADDR ADDR_FLASH_SECTOR_6
#define FLASH_STORAGE_AVAIL_FLAG 0xA5A5A5A5 //Flash数据有效标志

// 储存关节编码器校准参数
void Flash_SaveAngleCal(float cal_offset)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t error = 0;

	erase.Sector = FLASH_SECTOR_5;
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase.NbSectors = 1;

    HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase, &error);

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLAH_STORAGE_ANG_ADDR, FLASH_STORAGE_AVAIL_FLAG);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLAH_STORAGE_ANG_ADDR+4, (*((uint32_t*)&cal_offset)));
	
	HAL_FLASH_Lock();
}

int Flash_ReadAngleCal(float* cal_offset)
{
    uint32_t *buf = (uint32_t*)FLAH_STORAGE_ANG_ADDR;
    if(buf[0] != FLASH_STORAGE_AVAIL_FLAG)
		return -1;
    *cal_offset = *((float*)&buf[1]);
    return 0;
}

// 储存磁力计校准参数
void Flash_SaveMagCal(float magBias[3], float magScale[3])
{
	FLASH_EraseInitTypeDef erase;
    uint32_t error = 0;

	erase.Sector = FLASH_SECTOR_4;
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase.NbSectors = 1;

    HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase, &error);

    uint32_t buf[8];
    buf[0] = FLASH_STORAGE_AVAIL_FLAG;
	buf[1] = *((uint32_t*)magBias);
	buf[2] = *((uint32_t*)(magBias+1));
	buf[3] = *((uint32_t*)(magBias+2));
    buf[4] = *((uint32_t*)magScale);
	buf[5] = *((uint32_t*)(magScale+1));
	buf[6] = *((uint32_t*)(magScale+2));
    for(uint8_t i=0; i<sizeof(buf); i++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLAH_STORAGE_CAL_ADDR+i*4, buf[i]);
    }
	HAL_FLASH_Lock();
}

int Flash_ReadMagCal(float magBias[3], float magScale[3])
{
    uint32_t *buf = (uint32_t*)FLAH_STORAGE_CAL_ADDR;
	if(buf[0] != FLASH_STORAGE_AVAIL_FLAG)
		return -1;
	magBias[0] = *((float*)&buf[1]);
	magBias[1] = *((float*)&buf[2]);
	magBias[2] = *((float*)&buf[3]);
    magScale[0] = *((float*)&buf[4]);
	magScale[1] = *((float*)&buf[5]);
	magScale[2] = *((float*)&buf[6]);
    return 0;
}

void Flash_SaveMpuCal(float AccelScale, float GyroOffset[3])
{
    FLASH_EraseInitTypeDef erase;
    uint32_t error = 0;

	erase.Sector = FLASH_SECTOR_6;
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase.NbSectors = 1;

    HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase, &error);

    uint32_t buf[6];
    buf[0] = FLASH_STORAGE_AVAIL_FLAG;
	buf[1] = *((uint32_t*)&AccelScale);
    buf[2] = *((uint32_t*)GyroOffset);
	buf[3] = *((uint32_t*)(GyroOffset+1));
	buf[4] = *((uint32_t*)(GyroOffset+2));
    for(uint8_t i=0; i<sizeof(buf); i++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLAH_STORAGE_MPU_ADDR+i*4, buf[i]);
    }
	HAL_FLASH_Lock();
}

int Flash_ReadMpuCal(float *AccelScale, float GyroOffset[3])
{
    uint32_t *buf = (uint32_t*)FLAH_STORAGE_MPU_ADDR;
	if(buf[0] != FLASH_STORAGE_AVAIL_FLAG)
		return -1;
	*AccelScale = *((float*)&buf[1]);
    GyroOffset[0] = *((float*)&buf[2]);
	GyroOffset[1] = *((float*)&buf[3]);
	GyroOffset[2] = *((float*)&buf[4]);
    return 0;
}
