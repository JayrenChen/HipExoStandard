#include <Arduino.h>
// #include <HardwareSerial.h>
#include <module_ws2812.h>
#include <module_buzzer.h>
#include <module_button.h>
#include <module_battery.h>
#include <module_motor.h>
#include <module_vofa.h>
#include <module_ble.h>
#include <module_wifi.h>
#include <map_pin.h>
#include <ds_sys.h>

#define Serial_PC Serial0
#define Serial_VOICE Serial1

_Sys_State sys_state;

void task_serial_debug(void *pvParameters) {
  Serial_PC.begin(921600, SERIAL_8N1, PIN_UART0_RX, PIN_UART0_TX);
  Serial_VOICE.begin(115200, SERIAL_8N1, PIN_UART1_RX, PIN_UART1_TX);
  battery_init();

  while (1) {
    // Serial_PC.println("Serial 0");
    float_t voltage = battery_read();
    Serial_PC.printf("[SYS Info] Battery Voltage: %.2f V\n", voltage);
    Serial_VOICE.println("Voice Serial 1 Running...");
    delay(1000);
  }
}

/**
 * @brief 任务函数：RGB灯闪烁
 */
void task_flash_rgb(void *pvParameters) {
  
  // 初始化RGB灯
  rgb_init();
  rgb_ready();
  // rgb_rainbow(5);

  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  while (1) {
    rgb_blink();
    vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_PERIOD_MS);
  }
}

void task_beep_buzzer(void *pvParameters) {
  
  // 初始化蜂鸣器
  buzzer_init();
  buzzer_ready();

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    // buzzer_info();
    if (sys_state.sound_mode == 1) {
      buzzer_info();
      sys_state.sound_mode = 0;
    }
    vTaskDelayUntil(&xLastWakeTime, 20 / portTICK_PERIOD_MS);
  }
}

void task_button_push(void *pvParameters) {
  
  button_init();
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    button_loop();
    vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS);
  }
}

void task_motor_send(void *pvParameters) {

  Setup_MotorCan();

  // 电机控制任务
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    // 电机数据接受逻辑
    SendCanTest();
    vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_PERIOD_MS);
  }
}

void task_vofa_send(void *pvParameters) {

  Serial1.begin(921600);
  float send_data[10] = {0.0f};
  
  vTaskDelay(1000); // 等待电机初始化完成
  // VOFAs数据发送任务  
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    // VOFAs数据发送逻辑
    static float tick_timer = 0.0f; // 目标速度，单位为度/秒
    tick_timer += 0.01f;
    send_data[0] = tick_timer;
    send_data[1] = motor_state.pos;
    send_data[2] = motor_state.vel;
    send_data[3] = motor_state.tau;
    send_data[4] = motor_state.temp;
    Send_Vofa(send_data, 5); // 发送VOFA数据
    vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS);
  }
}

void task_ble(void *pvParameters)
{
    TickType_t xLastWakeTime;
    Setup_BLE("EXO_ESP32_BLE");
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        BLE_Process();
        vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS );
        xLastWakeTime = xTaskGetTickCount();
    }
}

void task_wifi(void *parm)
{
    TickType_t xLastWakeTime;
    Setup_Wifi_AP("EXO_ESP32_WIFI","12345678");
    Setup_UDP_IMU();
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        IMU_Process();
        vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_PERIOD_MS );
        xLastWakeTime = xTaskGetTickCount();
    }
}

void setup() {
  // 创建串口调试任务，调用函数，任务名称，堆栈大小，参数，优先级，任务句柄，核心ID
  xTaskCreatePinnedToCore(task_serial_debug, "SerialDebug", 4096, NULL, 1, NULL, 1);
  delay(1000);
  // 创建RGB闪烁任务，
  xTaskCreatePinnedToCore(task_flash_rgb, "RGB", 4096, NULL, 1, NULL, 1);
  // 创建蜂鸣器任务
  xTaskCreatePinnedToCore(task_beep_buzzer, "Buzzer", 4096, NULL, 1, NULL, 1);
  // 创建按键任务
  xTaskCreatePinnedToCore(task_button_push, "Button", 4096, NULL, 2, NULL, 1);
  // 创建电机数据发送任务
  xTaskCreatePinnedToCore(task_motor_send, "MotorSend", 4096, NULL, 1, NULL, 1);
  // 创建VOFA数据发送任务
  // xTaskCreatePinnedToCore(task_vofa_send, "VofaSend", 4096, NULL, 1, NULL, 1);
  // 创建BLE任务
  xTaskCreatePinnedToCore(task_ble, "BLE", 8192, NULL, 1, NULL, 1);
  // 创建WiFi任务
  xTaskCreatePinnedToCore(task_wifi, "WiFi", 8192, NULL, 1, NULL, 1);
}

void loop() {
  // Serial1.println("Main Loop Running...");
  delay(1000);
}

