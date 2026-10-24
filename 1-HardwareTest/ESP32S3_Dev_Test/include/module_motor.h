#ifndef _MODULE_MOTOR_H_
#define _MODULE_MOTOR_H_

#include <Arduino.h>

void Setup_MotorCan(void);
void Enable_Motor(void);
void Set_Motor_Speed(float des_vel);
void Rec_Motor_State(void);
void Set_Motor_Zero(void);
void Get_Motor_Version(void);
void Set_Motor_Tor(void);
void SendCanTest(void);

#endif