#ifndef __MODULE_WIFI_H
#define __MODULE_WIFI_H
#include <Arduino.h>

typedef union __IMUDataTypeDef
{
	int16_t  data[14];
	unsigned char b[28];
}_IMUDataTypeDef;

#define NUM_IMU 15

void Setup_Wifi_AP(const char *ssid, const char *password);
void Setup_Wifi(const char *ssid, const char *password);
void Setup_UDP_Local(const char * local_ip, int local_port);
void Setup_UDP_Host(const char * host_ip, int host_port);
void Send_UDP(const uint8_t *buffer, size_t size);
void Debug_UDP(const String &s);

void Setup_UDP_IMU(void);
void IMU_Process(void);

extern _IMUDataTypeDef IMU_Data[NUM_IMU];
extern uint8_t IMU_Ready[NUM_IMU];
extern uint16_t IMU_FPS[NUM_IMU];
#endif