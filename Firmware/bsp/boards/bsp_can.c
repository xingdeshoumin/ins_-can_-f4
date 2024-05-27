#include "bsp_can.h"

fchange_t  data4bytes;
gyro_param   	gyro_pitch = {0};

#define TEMP_CONFINE   70


void CAN_InitArgument(void)
{
    CAN_Enable(&hcan1);
}


void CAN_Enable(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_Start(hcan);//对can进行激活
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);//使能can接收中断
	CANFilter_Enable(hcan);//使能滤波器
}


void CANFilter_Enable(CAN_HandleTypeDef *hcan)
{
	CAN_FilterTypeDef filter1;
	if(hcan->Instance == CAN1)
	{
		filter1.FilterActivation=ENABLE;
		filter1.FilterBank=0U;
		filter1.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		filter1.FilterIdHigh=0x0000;
		filter1.FilterIdLow=0x0000;
		filter1.FilterMaskIdHigh=0x0000;
		filter1.FilterMaskIdLow=0x0000;
		filter1.FilterMode=CAN_FILTERMODE_IDMASK;
		filter1.FilterScale=CAN_FILTERSCALE_32BIT;
		filter1.SlaveStartFilterBank=14;
		
		HAL_CAN_ConfigFilter(&hcan1,&filter1);
	}
	
}

uint8_t mag_cal_flag;
uint8_t encoder_cal_flag;
uint8_t mpu_cal_flag;
uint8_t mag_en = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_Message can1_rx_message;
	CAN_RxHeaderTypeDef Can1RxHeader;
	if(hcan->Instance == CAN1)
    {
        HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&Can1RxHeader,can1_rx_message.Data);
        switch (Can1RxHeader.StdId)
        {
            case CAN_MAG_CAL_ID:
            {
                mag_cal_flag = can1_rx_message.Data[0];
                encoder_cal_flag = can1_rx_message.Data[1];
                mpu_cal_flag = can1_rx_message.Data[2];
                mag_en = can1_rx_message.Data[3];
            }
            break;
        }
	    
    }
}

void CAN_send_current(CAN_HandleTypeDef *hcan,uint32_t id,int16_t motor1, int16_t motor2, int16_t motor3,int16_t motor4)
{
    CAN_TxHeaderTypeDef  tx_message;
    uint8_t              can_send_data[8];
    uint32_t send_mail_box;
    tx_message.StdId = id;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = 0x08;
    can_send_data[0] = motor1 >> 8;
    can_send_data[1] = motor1;
    can_send_data[2] = motor2 >> 8;
    can_send_data[3] = motor2;
    can_send_data[4] = motor3 >> 8;
    can_send_data[5] = motor3;
    can_send_data[6] = motor4 >> 8;
    can_send_data[7] = motor4;
    
   HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, &send_mail_box);
} 

void CAN_send_data_2(CAN_HandleTypeDef *hcan,uint32_t id,int8_t data1,int8_t data2)
{
    CAN_TxHeaderTypeDef  tx_message;
    uint8_t              can_send_data[8];
    uint32_t send_mail_box;
    tx_message.StdId = id;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = 0x08;
    can_send_data[0] = data1;
    can_send_data[1] = data2;
    HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, &send_mail_box);

}

void CAN_send_data_4(CAN_HandleTypeDef *hcan,uint32_t id,int8_t data1,int8_t data2,int8_t data3,int8_t data4)
{
    CAN_TxHeaderTypeDef  tx_message;
    uint8_t              can_send_data[8];
    uint32_t send_mail_box;
    tx_message.StdId = id;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = 0x08;
    can_send_data[0] = data1;
    can_send_data[1] = data2;
    can_send_data[2] = data3;
    can_send_data[3] = data4;
    HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, &send_mail_box);

}


/**
  * @brief     get gyro data and unpack the data 
* @param     ptr: Pointer to a wl2data structure  ptrr: Pointer to a wl4data structure
  * @attention this function should be called after gyro is read
  */
void gyro_data_handle(fchange_t* ptrr,float* gyro,uint8_t RxData[])
{
	
    ptrr->c[0] = RxData[0];
    ptrr->c[1] = RxData[1];
    ptrr->c[2] = RxData[2];
    ptrr->c[3] = RxData[3];
    *gyro = ptrr->f;	
}

void BMI088_data_handle(uint8_t RxData[], float* TxData1, float* TxData2)
{
    fchange_t changer1, changer2;
    changer1.c[0] = RxData[0];
    changer1.c[1] = RxData[1];
    changer1.c[2] = RxData[2];
    changer1.c[3] = RxData[3];
    changer2.c[0] = RxData[4];
    changer2.c[1] = RxData[5];
    changer2.c[2] = RxData[6];
    changer2.c[3] = RxData[7];
    *TxData1 = changer1.f;
    *TxData2 = changer2.f;
}

uint8_t send_succeed = 0;
/*CAN发送1个4个元素的uint8_t数组，共32位(从float类型转换而来)*/
void CAN_send_data(CAN_HandleTypeDef *hcan,uint32_t id, uint8_t data[])
{
    CAN_TxHeaderTypeDef  tx_message;
    uint8_t              can_send_data[8];
    uint32_t send_mail_box;
    tx_message.StdId = id;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = 0x08;
    can_send_data[0] = data[0];
    can_send_data[1] = data[1];
    can_send_data[2] = data[2];
    can_send_data[3] = data[3];
    can_send_data[4] = data[4];
    can_send_data[5] = data[5];
    can_send_data[6] = data[6];
    can_send_data[7] = data[7];
	if(HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, &send_mail_box) == HAL_OK)
	{
		send_succeed = 1;
	}
	else
	{
		send_succeed = 2;
	}

}
