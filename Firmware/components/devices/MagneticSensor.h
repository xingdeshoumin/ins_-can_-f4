#ifndef MAGNETICSENSOR_LIB_H
#define MAGNETICSENSOR_LIB_H

/******************************************************************************/
extern long  cpr;
extern float full_rotation_offset;
extern long angle_data, angle_data_prev;
extern unsigned long velocity_calc_timestamp;
extern float angle_prev;
extern float cal_offset;
/******************************************************************************/
void MagneticSensor_Init(void);
void calAngle(float* cal_offset);
float getAngle(void);
float getVelocity(void);
/******************************************************************************/

#endif
