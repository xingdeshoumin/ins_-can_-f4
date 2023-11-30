#ifndef BSP_CAN_H
#define BSP_CAN_H
#include "can.h"
#include "stm32f4xx.h"
#include "INS_task.h"
#include "rm_send_task.h"

typedef enum
{
    CAN_MAG_CAL_ID = 0x101,
 	
} can_msg_id1;

typedef struct {
    uint16_t postion;
    int16_t speed;
} SMALL_MOTOR;

uint8_t MY_CAN_Enable(CAN_HandleTypeDef *hcan);
uint8_t CANTx_SendCurrent(CAN_HandleTypeDef *Target_hcan, uint32_t id, int16_t current1, int16_t current2, int16_t current3, int16_t current4);
void CAN_send_data(CAN_HandleTypeDef *hcan,uint32_t id, uint8_t data[]);

#endif  /* BSP_CAN_H */
