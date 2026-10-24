#ifndef __MODULE_VOFA_
#define __MODULE_VOFA_
#include <Arduino.h>

#define MAX_VOFA_LEN 60

typedef union __VofaSendDataTypeDef
{
	float  data[MAX_VOFA_LEN+1];
	unsigned char b[(MAX_VOFA_LEN+1)*4];
}_VofaSendDataTypeDef;


void Set_VOFA_Data4(float data0, float data1, float data2, float data3);
void Send_VOFA_Wifi(uint8_t len);
void Send_VOFA_Serial1(uint8_t len);
void Send_VOFA_Serial2(uint8_t len);
void Send_VOFA_Serial_IMU(uint8_t len);
void VOFA_Process(void);

extern volatile _VofaSendDataTypeDef vofa_send_t;

#endif