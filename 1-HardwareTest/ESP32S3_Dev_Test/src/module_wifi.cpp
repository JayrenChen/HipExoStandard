/**
 * @file module_wifi.cpp
 * @author JYChen
 * @brief ESP32S3 Wifi UDP通信模块
 * @version 0.1
 * @date 2025-03-01: 创建文件
 * @date 2025-04-09: 添加Debug_UDP函数
 * @copyright Copyright (c) 2025
 * 
 */
#include <module_wifi.h>
#include <WiFi.h>
#include <WiFiUDP.h>
// #include <module_rgb.h>
#include <module_vofa.h>

WiFiUDP Udp;
int HostPort = 8080;
IPAddress HostIP;
IPAddress subnet(255,255,255,0); //子网掩码
bool flag_wifi = false;

IPAddress DebugIP;
int DebugPort = 8080;

IPAddress localIP = IPAddress(192,168,4,1); //本地IP地址
IPAddress gateway = IPAddress(192,168,4,1); //网关地址

WiFiUDP IMU[NUM_IMU];
_IMUDataTypeDef IMU_Data[NUM_IMU];
uint8_t IMU_Ready[NUM_IMU] = {0};
uint16_t IMU_FPS_Counter[NUM_IMU] = {0};
uint16_t IMU_FPS[NUM_IMU] = {0};

/**
 * @brief 连接Wifi网络
 * @author JYChen
 * @param ssid WiFi名称
 * @param password WiFi密码
 * @return None
 */
void Setup_Wifi_AP(const char *ssid, const char *password)
{
	WiFi.mode(WIFI_AP); //设置工作在AP模式
	WiFi.softAPConfig(localIP, gateway, subnet); //设置AP地址

	while(!WiFi.softAP(ssid, password, 1, 0, 15, false)){}; //启动AP,并设置最大连接数15
	WiFi.config(localIP, gateway, subnet);

	Serial.println("AP启动成功");

	Serial.print("主机名:");
	Serial.println(WiFi.softAPgetHostname());
	Serial.print("主机IP:");
	Serial.println(WiFi.softAPIP());
	Serial.print("主机IPV6:");
	Serial.println(WiFi.softAPIPv6());
	Serial.print("主机SSID:");
	Serial.println(WiFi.SSID());
	Serial.print("主机广播IP:");
	Serial.println(WiFi.softAPBroadcastIP());
	Serial.print("主机mac地址:");
	Serial.println(WiFi.softAPmacAddress());
	Serial.print("主机连接个数:");
	Serial.println(WiFi.softAPgetStationNum());
	Serial.print("主机网络ID:");
	Serial.println(WiFi.softAPNetworkID());
	Serial.print("主机状态:");
	Serial.println(WiFi.status());
	delay(1000);

}

/**
 * @brief 连接Wifi网络
 * @author JYChen
 * @param ssid WiFi名称
 * @param password WiFi密码
 * @return None
 */
void Setup_Wifi(const char *ssid, const char *password)
{
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) 
	{
		flag_wifi = !flag_wifi;
		// RGB_Blink_Blue_100ms(); // 连接过程蓝灯闪烁
	}
	// RGB_Blink_Blue_1000ms(); // 连接完成绿灯长闪一次
}

/**
 * @brief 设置本地网络地址和端口
 * @author JYChen
 * @param local_ip 本地IP地址
 * @param local_port 本地IP端口
 * @return None
 */
void Setup_UDP_Local(const char * local_ip, int local_port)
{
	IPAddress localIP;
	localIP.fromString(local_ip);
	WiFi.config(localIP, localIP, subnet);
	Udp.begin(local_port);
}

/**
 * @brief 设置主机地址和端口
 * @author JYChen
 * @param host_ip 主机IP地址
 * @param host_port 主机IP端口
 * @return None
 */
void Setup_UDP_Host(const char * host_ip, int host_port)
{
	HostIP.fromString(host_ip);
	HostPort = host_port;
}

/**
 * @brief 向主机和相应端口通过UDP协议发送数据
 * @author JYChen
 * @param buffer 字节数据的地址
 * @param size 字节数据长度
 */
void Send_UDP(const uint8_t *buffer, size_t size)
{
	if(flag_wifi)
	{
		Udp.beginPacket(HostIP, HostPort); 
		Udp.write(buffer, size); 
		Udp.endPacket();
	}
}

/**
 * @brief 设置Debug信息发送的主机地址和端口
 * @author JYChen
 * @param debug_ip Debug主机IP地址
 * @param debug_port Debug主机IP端口
 * @return None
 */
void Setup_Debug(const char * debug_ip, int debug_port)
{
	DebugIP.fromString(debug_ip);
	DebugPort = debug_port;
}


/**
 * @brief 向主机和相应端口通过UDP协议发送Debug信息
 * @author JYChen
 * @param s 字符串，暂时不支持标准化输出
 */
void Debug_UDP(const String &s)
{
	if(flag_wifi)
	{
		Udp.beginPacket(HostIP, HostPort); 
		Udp.write((uint8_t *)s.c_str(), s.length());
		Udp.write('\r');Udp.write('\n');
		Udp.endPacket();
	}
}

// /**
//  * @brief 设置本地网络地址和端口
//  * @author JYChen
//  * @param local_ip 本地IP地址
//  * @param local_port 本地IP端口
//  * @return None
//  */
// void Setup_UDP_Local(const char * local_ip, int local_port)
// {
// 	IPAddress localIP;
// 	localIP.fromString(local_ip);
// 	WiFi.config(localIP, localIP, subnet);
// 	Udp.begin(local_port);
// }


void Setup_UDP_IMU(void)
{
	for(int i = 0; i < NUM_IMU; i++)
	{
		IMU[i].begin(9001 + i);
		delay(100);
		Serial.printf("IMU%d UDP端口打开成功\r\n", i+1);
	}
}

void IMU_Process(void)
{
	static uint8_t imuPacket[256];
	static int len;
	static int fps_timer = 0;

	for(int i = 0; i < NUM_IMU; i++)
	{
		if(IMU[i].parsePacket())
		{
			// 读取数据包
			len = IMU[i].read(imuPacket, 256);
			// 处理接收到的数据
			memcpy(IMU_Data[i].b, imuPacket+20, 28);
			IMU_FPS_Counter[i]++; // 统计FPS
			// vofa_send_t.data[0] += IMU_FPS[i];
			// vofa_send_t.data[++send_frame_count] = IMU_Data[i].data[0]/32768.0*16.0;
			// if(i == 0)
			// {
			// 	vofa_send_t.data[0] = IMU_FPS[i];
			// 	vofa_send_t.data[1] = IMU_Data[i].data[0]/32768.0*16.0;
			// 	vofa_send_t.data[2] = IMU_Data[i].data[1]/32768.0*16.0;
			// 	vofa_send_t.data[3] = IMU_Data[i].data[2]/32768.0*16.0;
			// 	Send_VOFA_Serial_IMU(4);
			// }
		}
	}

	// 计算IMU FPS
	if(++fps_timer >= 500)
	{
		fps_timer = 0;
		for(int i = 0; i < NUM_IMU; i++)
		{
			IMU_FPS[i] = IMU_FPS_Counter[i];
			IMU_FPS_Counter[i] = 0;
			// Serial.printf("IMU%d FPS: %d\r\n", i+1, IMU_FPS[i]);
		}
	}

}