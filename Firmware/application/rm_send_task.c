
#include "rm_send_task.h"

void Float_to_Byte(float a,float b,unsigned char byte[]);

extern uint8_t angle_cal_flag;
extern fp32 INS_angle[3];
fp32 encoder_angle;
uint8_t byte_0[8];
uint8_t angle_cal_flag_last;

void SendTask(void const * argument)
{
  /* USER CODE BEGIN SendTask */
  MagneticSensor_Init(); //初始化磁传感器
  CAN_InitArgument();
  Flash_ReadAngleCal(&cal_offset);
  /* Infinite loop */
  for(;;)
  {
#ifdef RIGHT_UP_LEG
    Float_to_Byte(INS_angle[0], INS_angle[1], byte_0);
    CAN_send_data(&hcan1, 0x01, byte_0);
    Float_to_Byte(INS_angle[2], encoder_angle,byte_0);
    CAN_send_data(&hcan1, 0x02, byte_0);
#endif
#ifdef LEFT_UP_LEG
    Float_to_Byte(INS_angle[0], INS_angle[1], byte_0);
    CAN_send_data(&hcan1, 0x03, byte_0);
    Float_to_Byte(INS_angle[2], encoder_angle,byte_0);
    CAN_send_data(&hcan1, 0x04, byte_0);
#endif
#ifdef LEFT_DOWN_LEG
    Float_to_Byte(INS_angle[0], INS_angle[1], byte_0);
    CAN_send_data(&hcan1, 0x05, byte_0);
    Float_to_Byte(INS_angle[2], encoder_angle,byte_0);
    CAN_send_data(&hcan1, 0x06, byte_0);
#endif
#ifdef RIGHT_DOWN_LEG
    Float_to_Byte(INS_angle[0], INS_angle[1], byte_0);
    CAN_send_data(&hcan1, 0x07, byte_0);
    Float_to_Byte(INS_angle[2], encoder_angle,byte_0);
    CAN_send_data(&hcan1, 0x08, byte_0);
#endif
#ifdef BODY
    Float_to_Byte(INS_angle[0], INS_angle[1], byte_0);
    CAN_send_data(&hcan1, 0x09, byte_0);
    Float_to_Byte(INS_angle[2], encoder_angle,byte_0);
    CAN_send_data(&hcan1, 0x10, byte_0);
#endif
    if (angle_cal_flag)
    {
        cal_offset = getAngle();
    }
    else
    {
        encoder_angle = fabs((getAngle() - cal_offset));
    }
    if (angle_cal_flag_last && !angle_cal_flag)
    {
        Flash_SaveAngleCal(cal_offset);
    }
    angle_cal_flag_last = angle_cal_flag;
    osDelay(1);
  }
  /* USER CODE END SendTask */
}

//定义共用体，参考https://blog.csdn.net/ls667/article/details/50811519博客
typedef union     
{
	float fdata;
	unsigned long ldata;
}FloatLongType;


/*将浮点数f转化为4个字节数据存放在byte[4]中*/
void Float_to_Byte(float a,float b,unsigned char byte[])   //参考https://blog.csdn.net/ls667/article/details/50811519博客
{
	FloatLongType fl,f2;
	fl.fdata=a;f2.fdata=b;
	byte[0]=(unsigned char)fl.ldata;
	byte[1]=(unsigned char)(fl.ldata>>8);
	byte[2]=(unsigned char)(fl.ldata>>16);
	byte[3]=(unsigned char)(fl.ldata>>24);
    byte[4]=(unsigned char)f2.ldata;
	byte[5]=(unsigned char)(f2.ldata>>8);
	byte[6]=(unsigned char)(f2.ldata>>16);
	byte[7]=(unsigned char)(f2.ldata>>24);
}
