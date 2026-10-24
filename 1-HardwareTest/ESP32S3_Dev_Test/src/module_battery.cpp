#include <module_battery.h>
#include <map_pin.h>

float battery_voltage = 14.8; // 电池电压，单位：伏特
float battery_gain = 5.99f; // 分压电路增益 10k vs 49.9k
float filter_alpha = 0.1f; // 滤波系数，范围为0~1，值越小滤波效果越明显

void battery_init(void) {
    // 初始化电池模块
    analogReadResolution(12);
    battery_voltage = analogReadMilliVolts(PIN_BATTERY) / 1000.0 * battery_gain;
    ESP_LOGI("DRV", "Battery初始化完成✅");
}

float_t battery_read(void) {
    // 读取电池电压
    float voltage = analogReadMilliVolts(PIN_BATTERY) / 1000.0 * battery_gain;
    battery_voltage = filter_alpha * voltage + (1 - filter_alpha) * battery_voltage;
    return battery_voltage;
}