#ifndef DS_SYS_H
#define DS_SYS_H

#include <Arduino.h>

#define MAX_VOFA_LEN 60
#define CYBERGEAR
// #define DJM2006
// #define QZB_Basic

typedef union __CyberGearMotorDataTypeDef
{
    uint16_t  parm[4];
    unsigned char b[8];
}_CyberGearMotorDataTypeDef;

typedef struct __CyberGearMotorStateTypeDef
{
    float pos;
    float bias;
    float vel;
    float tau;
    float temp;
}_CyberGearMotorStateTypeDef;

typedef struct __SYS_STATE{
    float bat_vol;
    uint8_t light_mode;
    uint8_t sound_mode;
    uint8_t button_state;
    // uint8_t ota_state;
    // uint8_t wifi_mode;
}_Sys_State;

typedef union __VofaSendDataTypeDef
{
	float  data[MAX_VOFA_LEN+1];
	unsigned char b[(MAX_VOFA_LEN+1)*4];
}_VofaSendDataTypeDef;

extern _Sys_State sys_state;
extern volatile _VofaSendDataTypeDef vofa_send_t;
extern _CyberGearMotorStateTypeDef motor_state;

#endif