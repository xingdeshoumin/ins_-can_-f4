/**************************************************************************************** 
  * @file       bsp_can.c
  * @brief      Initialization of CAN communication and receive data from the CAN bus
  * @author     CHEN Shu
  ***************************************************************************************
  * Version     Date           Author        Modification
  * V1.0.0      Dec-04-2020    CHEN Shu      未完成
  * -------------------------- Personal Add ---------------------------------------------
  * @initialize 
  * @funtion    
  * @note       
  ***************************************************************************************
  */
  
/****************************** Private includes *******************************/

#include "bsp_can.h"

/****************************** Private defines ********************************/
SMALL_MOTOR motor;

/**************************** Functions declaration ****************************/
static void CANFilter_Enable(CAN_HandleTypeDef* hcan);

/****************************** Private functions ******************************/
// Some initialization function --------------------------
/**
  * @brief     Enable CAN1 and CAN2
  * @param     None
  * @return    0 if succeed/ 1 if failed
  */
uint8_t MY_CAN_Enable(CAN_HandleTypeDef *hcan)
{
    CANFilter_Enable(hcan);  //使能滤波器
    HAL_CAN_Start(hcan);  //对CAN进行激活
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);  //使能CAN接收中断

    return 0;
}


/**
 * @brief     Enable filter
 * @param     None
 * @return    None
 * @attention filter bank 0 for CAN1 and filter bank 14 for CAN2
 */
static void CANFilter_Enable(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef filter1;

	if (hcan->Instance == CAN1)
	{
		filter1.FilterActivation=ENABLE;
		filter1.FilterBank=14;
		filter1.FilterFIFOAssignment=CAN_FILTER_FIFO0;
		filter1.FilterIdHigh=0x0000;
		filter1.FilterIdLow=0x0000;
		filter1.FilterMaskIdHigh=0x0000;
		filter1.FilterMaskIdLow=0x0000;
		filter1.FilterMode=CAN_FILTERMODE_IDMASK;
		filter1.FilterScale=CAN_FILTERSCALE_32BIT;
		filter1.SlaveStartFilterBank=14;
		
		HAL_CAN_ConfigFilter(&hcan1, &filter1);
	}

}


// CAN transmit data function --------------------------------
/**
  * @brief  Sent current data to electronic speed contrlloer by 'CAN_Transmit_IT'
  * @param  <Target_hcan>: The handle pointer of the CAN port we are using
  * 		<current1>: 1 to 2 byte of data to the motor whose ID is 0x201, ranging [-16384,16384]
  * 		<current2>: 3 to 4 byte of data to the motor whose ID is 0x202, ranging [-16384,16384]
  * 		<current3>: 5 to 6 byte of data to the motor whose ID is 0x203, ranging [-10000,10000]
  * 		<current4>: Reset
  * @retval 0 if succeed/ 1 if failed
  */
uint8_t CANTx_SendCurrent(CAN_HandleTypeDef* Target_hcan, uint32_t id, int16_t current1, int16_t current2, int16_t current3, int16_t current4)
{
    //声明存储发送数据的结构体
    uint8_t EC_message[8] = {0};
    //定义 CAN 的发送消息句柄
    CAN_TxHeaderTypeDef CanTxHeader;
    //定义发送邮箱
    uint32_t TX_MailBOX = CAN_TX_MAILBOX0;

    //配置 CAN 的发送消息句柄
    CanTxHeader.StdId = id;
    CanTxHeader.IDE = CAN_ID_STD;
    CanTxHeader.RTR = CAN_RTR_DATA;
    CanTxHeader.DLC = 0x08;

    EC_message[0] = (unsigned char)(current1 >> 8);
    EC_message[1] = (unsigned char)current1;
    EC_message[2] = (unsigned char)(current2 >> 8);
    EC_message[3] = (unsigned char)current2;
    EC_message[4] = (unsigned char)(current3 >> 8);
    EC_message[5] = (unsigned char)current3;
    EC_message[6] = (unsigned char)(current4 >> 8);
    EC_message[7] = (unsigned char)current4;

    //将发送的信息添加到信箱，之后发送
    if(HAL_CAN_AddTxMessage(Target_hcan, &CanTxHeader, EC_message, &TX_MailBOX) != HAL_OK)
    {
        return 1;
    }
    return 0;
}

uint16_t cccc;
// CAN interruption callback function ------------------------------
/**
 * @brief     interrupt function in IRQ
 * @param     None
 * @return    None
 * @attention None
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	static uint8_t CAN1_RX_Buff[8], CAN2_RX_Buff[8];
	CAN_RxHeaderTypeDef Can1RxHeader,Can2RxHeader; //header we need to receive the data from CAN
	//if the interruption source is CAN1, mainly the chassis
	if (hcan->Instance == CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &Can1RxHeader, CAN1_RX_Buff); //fetch data from FIFO to 'CANx_RX_Buff'
		//judge the ID to check which motor
		switch (Can1RxHeader.StdId)
		{
            case 0x202: {
//                motor.postion = CAN1_RX_Buff[0]<<8 | CAN1_RX_Buff[1];
//                motor.speed = CAN1_RX_Buff[2]<<8 | CAN1_RX_Buff[3];
                break;
            }
		}
	}
	// if (hcan->Instance == CAN1)
	// {
	// 	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&Can2RxHeader,CAN2_RX_Buff);
	// 	switch (Can2RxHeader.StdId)
	// 	{
    //         case 0x202: {
               
    //             break;
    //         }
	// 	}
	// }
    cccc++;//自增，判断是否成功进入CAN中断回调
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
