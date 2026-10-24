/**
 * @file module_vofa.cpp
 * @author JYChen
 * @brief VOFA通信协议
 * @version 0.1
 * @date 2025-07-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <module_vofa.h>
#include <string.h>
#include <ds_sys.h>

// 构造vofa_send_t数据结构体
volatile _VofaSendDataTypeDef vofa_send_t;
#define SendTool "Serial1"

void Send_Data(uint8_t len)
{
    vofa_send_t.b[4*len] = 0x00;
    vofa_send_t.b[4*len+1] = 0x00;
    vofa_send_t.b[4*len+2] = 0x80;
    vofa_send_t.b[4*len+3] = 0x7f;
    if (strcmp(SendTool, "Serial1") == 0)
        Serial1.write((uint8_t*)vofa_send_t.b, (len+1)*4);
    else if (strcmp(SendTool, "Serial2") == 0)
        Serial2.write((uint8_t*)vofa_send_t.b, (len+1)*4);
    // else if (strcmp(SendTool, "Wifi") == 0)
    //     Send_UDP((uint8_t*)vofa_send_t.b, (len+1)*4);
}

void Send_Vofa(float * data, uint8_t len)
{
    if(len > MAX_VOFA_LEN) return;
    for(int i=0; i<len; i++) vofa_send_t.data[i] = data[i];
    Send_Data(len);
}

/**
 * @brief 通过WIFI发送VOFA数据
 * 
 * @param len 数据长度
 */
// void Send_VOFA_Wifi(uint8_t len)
// {
//     vofa_send_t.b[4*len] = 0x00;
//     vofa_send_t.b[4*len+1] = 0x00;
//     vofa_send_t.b[4*len+2] = 0x80;
//     vofa_send_t.b[4*len+3] = 0x7f;
//     Send_UDP((uint8_t*)vofa_send_t.b, (len+1)*4);
// }


