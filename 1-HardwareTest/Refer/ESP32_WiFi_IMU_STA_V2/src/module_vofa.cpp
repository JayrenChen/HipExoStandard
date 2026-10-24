/**
 * @file module_vofa.cpp
 * @author JYChen
 * @brief VOFA通信协议
 * @version 0.1
 * @date 2025-03-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <module_vofa.h>
#include <module_wifi.h>

extern bool flag_wifi;
// 构造vofa_send_t数据结构体
volatile _VofaSendDataTypeDef vofa_send_t;

/**
 * @brief 测试函数，写入VOFA四参数
 * 
 * @param data0 
 * @param data1 
 * @param data2 
 * @param data3 
 */
void Set_VOFA_Data4(float data0, float data1, float data2, float data3)
{
    vofa_send_t.data[0] = data0;
    vofa_send_t.data[1] = data1;
    vofa_send_t.data[2] = data2;
    vofa_send_t.data[3] = data3;
    Send_VOFA_Wifi(4);
}

/**
 * @brief 通过WIFI发送VOFA数据
 * 
 * @param len 数据长度
 */
void Send_VOFA_Wifi(uint8_t len)
{
    vofa_send_t.b[4*len] = 0x00;
    vofa_send_t.b[4*len+1] = 0x00;
    vofa_send_t.b[4*len+2] = 0x80;
    vofa_send_t.b[4*len+3] = 0x7f;
    Send_UDP((uint8_t*)vofa_send_t.b, (len+1)*4);
}

/**
 * @brief 通过Serial1发送VOFA数据
 * 
 * @param len 数据长度
 */
void Send_VOFA_Serial1(uint8_t len)
{
    vofa_send_t.b[4*len] = 0x00;
    vofa_send_t.b[4*len+1] = 0x00;
    vofa_send_t.b[4*len+2] = 0x80;
    vofa_send_t.b[4*len+3] = 0x7f;
    Serial1.write((uint8_t*)vofa_send_t.b, (len+1)*4);
}

/**
 * @brief 通过Serial2发送VOFA数据
 * 
 * @param len 数据长度
 */
void Send_VOFA_Serial2(uint8_t len)
{
    vofa_send_t.b[4*len] = 0x00;
    vofa_send_t.b[4*len+1] = 0x00;
    vofa_send_t.b[4*len+2] = 0x80;
    vofa_send_t.b[4*len+3] = 0x7f;
    Serial2.write((uint8_t*)vofa_send_t.b, (len+1)*4);
}

/**
 * @brief 通过Serial1发送VOFA数据
 * 
 * @param len 数据长度
 */
void Send_VOFA_Serial_IMU(uint8_t len)
{
    vofa_send_t.b[4*len] = 0x00;
    vofa_send_t.b[4*len+1] = 0x00;
    vofa_send_t.b[4*len+2] = 0x80;
    vofa_send_t.b[4*len+3] = 0x7f;
    Serial.write((uint8_t*)vofa_send_t.b, (len+1)*4);
}

void VOFA_Process(void)
{
    for(int i = 0; i < 6; i++)
    {
        vofa_send_t.data[i] = IMU_Data[i].data[0]/32768.0*16.0;
    }
    for(int i = 0; i < 6; i++)
    {
        vofa_send_t.data[i+6] = IMU_FPS[i];
    }
    Send_VOFA_Serial_IMU(12);
}