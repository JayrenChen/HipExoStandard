/**
 * @file main.cpp
 * @author JYChen
 * @brief 外骨骼数据接收
 * @version 0.1
 * @date 2025-03-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <Arduino.h>
#include <module_rgb.h> // RGB模块
#include <BLEDevice.h> // 蓝牙Ble设备库
#include <BLEUtils.h> 
#include <BLEScan.h> // 蓝牙ble设备的扫描功能库
#include <BLEAdvertisedDevice.h> // 扫描到的蓝牙设备（广播状态）
#include <BLE2902.h>
#include <Ticker.h> // 是ESP32 Arduino固件自带的库
// #include <sensor.h>
#include <module_wifi.h> // Wifi模块
#include <module_vofa.h> // VOFA协议模块
#include <module_ble.h> // BLE模块
#include <OneButton.h>
#include <ezBuzzer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

OneButton button(9, true);
ezBuzzer buzzer(7, BUZZER_TYPE_PASSIVE, HIGH);
void singleClick(void);
void doubleClick(void);
void longPress(void);
void multiClick(void);
void getVol(void);

// RGB模式选择
uint8_t RGB_MODE = RGB_Mode_Rainbow_Cycle; // 1: Rainbow Cycle  2: Breathe Cycle
uint8_t BUZZER_MODE = 0; // 0: OFF  1: ON
bool TASK_START = false;

float BatVol  = 3.7; // 电池电压
float HostVol = 3.7; // 电池电压


void Buzzer_Task(void *parm)
{
    buzzer.setBeepFrequency(1000);

    if(HostVol > 4.0)
        buzzer.beep(1000);
    else
        buzzer.beep(200);

    while(1)
    {
        if(BUZZER_MODE == 1)
        {
            buzzer.beep(200);
            BUZZER_MODE = 0;
        }
        buzzer.loop();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void RGB_Task(void *parm)
{
    Setup_RGB_LINE();

    if(HostVol > 4.0)
        RGB_MODE = RGB_Mode_Charge_Cycle;
    else
        RGB_MODE = RGB_Mode_Rainbow_Cycle;

    while(1)
    {
        switch(RGB_MODE)
        {
            case RGB_Mode_IMU_Cycle:
                Process_RGB();
                break;
            case RGB_Mode_Rainbow_Cycle:
                Rainbow_Cycle();
                break;
            case RGB_Mode_Breathe_Cycle:
                Breathe_Cycle();
                break;
            case RGB_Mode_Light_ON:
                Light_ON();
                break;
            case RGB_Mode_Light_OFF:
                Light_OFF();
                break;
            case RGB_Mode_Charge_Cycle:
                Charge_Cycle();
                break;
            default:
                Rainbow_Cycle();
                break;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void IMU_Task(void *parm)
{
    TickType_t xLastWakeTime;
    Setup_Wifi_AP("ESP32_RC_HC25","12345678");
    Setup_UDP_IMU();
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        IMU_Process();
        vTaskDelayUntil(&xLastWakeTime, 2 / portTICK_PERIOD_MS );
        xLastWakeTime = xTaskGetTickCount();
    }
}

void VOFA_Task(void *parm)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        VOFA_Process();
        vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS );
        xLastWakeTime = xTaskGetTickCount();
    }
}

void BLE_Task(void *parm)
{
    TickType_t xLastWakeTime;
    Setup_BLE("IMU_STA_001");
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        BLE_Process();
        vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS );
        xLastWakeTime = xTaskGetTickCount();
    }
}

void CPU_Task(void *arg)
{
    // /*
    //  vTaskGetRunTimeStats() 使用
    //  注意:
    //  使用 vTaskGetRunTimeStats() 前需使能:
    //  make menuconfig -> Component config -> FreeRTOS -> configUSE_TRACE_FACILITY
    //  make menuconfig -> Component config -> FreeRTOS -> Enable FreeRTOS trace facility -> configUSE_STATS_FORMATTING_FUNCTIONS
    //  make menuconfig -> Component config -> FreeRTOS -> Enable display of xCorelD in vlaskList
    //  make menuconfig -> Component config -> FreeRTOS -> configGENERATE_RUN_TIME_STATS
    //  通过上面配置，等同于使能 FreeRTOSConfig.h 中如下三个宏:
    //  configGENERATE_RUN_TIME_STATS, configUSE_STATS_FORMATTING_FUNCTIONS 和 configSUPPORT_DYNAMIC_ALLOCATION
    // */
 
    //  uint8_t CPU_RunInfo[400]; //保存任务运行时间信息
 
    //  while (1)
    // {
    //     memset(CPU_RunInfo, 0, 400); /* 信息缓冲区清零 */

    //     vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息

    //     printf("----------------------------------------------------\r\n");
    //     printf("task_name     task_status     priority stack task_id\r\n");
    //     printf("%s", CPU_RunInfo);
    //     printf("----------------------------------------------------\r\n");
 
    //     memset(CPU_RunInfo, 0, 400); /* 信息缓冲区清零 */
 
    //     vTaskGetRunTimeStats((char *)&CPU_RunInfo);

    //     printf("task_name      run_cnt                 usage_rate   \r\n");
    //     printf("%s", CPU_RunInfo);
    //     printf("----------------------------------------------------\r\n");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}

void setup() {

    pinMode(8,OUTPUT); digitalWrite(8,HIGH); 

    Serial.begin(921600);
    getVol();

    pinMode(9,INPUT);
    button.setup(9, INPUT, false); // 设置按钮引脚为输入并启用上拉电阻
    // Attach the functions to be called on button events
    button.attachClick(singleClick);
    button.attachDoubleClick(doubleClick);
    button.attachLongPressStart(longPress);
    button.attachMultiClick(multiClick);
    button.setLongPressIntervalMs(1500);


    // --------------   任务函数, 优先级, 栈大小,传参,优先级,任务句柄,运行核心 -------------------
    xTaskCreatePinnedToCore(CPU_Task,"CPU_Task",4096,NULL,1,NULL,1);
    xTaskCreatePinnedToCore(RGB_Task,"RGB_Task",4096,NULL,1,NULL,1);
    xTaskCreatePinnedToCore(Buzzer_Task,"Buzzer_Task",4096,NULL,1,NULL,1);
    
    if(HostVol < 4.0)
    {
        TASK_START = true;
        // --------------   任务函数, 优先级, 栈大小,传参,优先级,任务句柄,运行核心 -------------------
        xTaskCreatePinnedToCore(IMU_Task,"IMU_Task",4096,NULL,1,NULL,1);
        xTaskCreatePinnedToCore(BLE_Task,"BLE_Task",4096,NULL,1,NULL,1);
        xTaskCreatePinnedToCore(VOFA_Task,"VOFA_Task",4096,NULL,1,NULL,1);
    }
    else
    {
        TASK_START = false;
        Serial.println("Charge Mode, IMU, BLE, VOFA Tasks not started.");
    }
    
}

void loop() {
    
    getVol();
    button.tick();
    delay(10);

}

void singleClick(void)
{
    BUZZER_MODE = 1;
    Serial.println("\t - SingleClick()");
    RGB_MODE = (RGB_MODE % 2) + 1; // 切换模式
}
void doubleClick(void)
{
    BUZZER_MODE = 1;
    Serial.println("\t - DoubleClick()");
    // RGB_MODE = 0; // 切换模式
    RGB_MODE = (RGB_MODE % 2) + 1 + 2; // 切换模式
}
void longPress(void)
{
    Serial.println("\t - LongPress()\n");
    RGB_MODE = RGB_Mode_Light_OFF;
    BUZZER_MODE = 1;
    Light_OFF();
    delay(100);
    digitalWrite(8,LOW); 
}
void multiClick(void)
{
    Serial.println("\t - MultiClick()");
    uint8_t clickCount = button.getNumberClicks();
    if(clickCount == 3)
    {
        BUZZER_MODE = 1;
        RGB_MODE = RGB_Mode_Charge_Cycle;
    }
    if(clickCount == 4)
    {
        BUZZER_MODE = 1;
        RGB_MODE = RGB_Mode_IMU_Cycle;
        if(TASK_START == false)
        {
            xTaskCreatePinnedToCore(IMU_Task,"IMU_Task",4096,NULL,1,NULL,1);
            xTaskCreatePinnedToCore(BLE_Task,"BLE_Task",4096,NULL,1,NULL,1);
            xTaskCreatePinnedToCore(VOFA_Task,"VOFA_Task",4096,NULL,1,NULL,1);
            TASK_START = true;
            Serial.println("IMU, BLE, VOFA Tasks started.");
        }
    }
        
}

void getVol(void)
{
    static float rawBat = 0;
    static float rawHost = 0;
    static float a = 0.8;
    static uint16_t disp_cnt = 100;

    if(rawBat == 0)
    {
        for(int i=0; i<10; i++)
        {
            rawBat  += analogRead(12) * 2;
            rawHost += analogRead(13) * 2;
            delay(10);
        }
        rawBat  /= 10;
        rawHost /= 10;
    }
    else
    {
        rawBat  = (a * rawBat  + (1 - a) * analogRead(12) * 2);
        rawHost = (a * rawHost + (1 - a) * analogRead(13) * 2);
    }

    BatVol  = (float)(rawBat  / 4095.0 * 3.3 ); // 电池电压
    HostVol = (float)(rawHost / 4095.0 * 3.3 ); // 主机电压

    if(++disp_cnt > 100)
    {
        Serial.printf("BatVol: %.2f V | HostVol: %.2f V\n", BatVol, HostVol);
        disp_cnt = 0;
    }
    
}
