#ifndef __MODULE_APP_H
#define __MODULE_APP_H
#include <Arduino.h>

typedef union __AppSendInfoType
{
    uint8_t b[10];
    int16_t data[5];
}_AppSendInfoType;

typedef union __AppSendDataType
{
    uint8_t b[512];
    int16_t data[256];
}_AppSendDataType;

typedef union __FirmwareSize
{
    uint8_t b[4];
    int32_t data;
}_FirmwareSize;

typedef struct __AppRecDataType
{
    uint8_t rec_cmd;
    uint8_t rec_parm;
    uint8_t rec_flag;
}_AppRecDataType;

typedef union __BleMasterSendDataType
{
    uint8_t b[40];
    int16_t data[20];
}_BleMasterSendDataType;

#pragma pack(push, 1)
typedef struct __ImuRawBleDataType
{
    uint8_t head1;
    uint8_t head2;
    uint32_t idx;
    float x;
    float y;
    float z;
    float w;
    float ax;
    float ay;
    float az;
    uint8_t tail1;
    uint8_t tail2;
}_ImuRawBleDataType;
#pragma pack(pop)

typedef union __ImuBleDataType
{
    _ImuRawBleDataType data;
    uint8_t b[36];
}_ImuBleDataType;

typedef struct __QuaternionDataType
{
    float x;
    float y;
    float z;
    float w;
}_QuatType;

void Init_BLE_Server(void);
void Send_App_Data(int16_t,int16_t,int16_t);
void Setup_BLE(std::string deviceName);
void Get_App_Cmd(void);

void Send_App_Info();
void Send_App_Data(uint8_t *data, uint16_t len);

void BLE_Process();

#endif