#include <Arduino.h>
#include <module_buzzer.h>
#include <map_pin.h>

uint8_t PWM_CHANNEL = 1;
uint32_t PWM_frequency = NOTE_E7;
uint8_t PWM_resolution = 12;
uint32_t BEAT_TIME = 1000;
uint32_t GAP_TIME = 50;

void buzzer_init(void)
{
    pinMode(PIN_BUZZER, OUTPUT);

    ledcSetup(PWM_CHANNEL, PWM_frequency, PWM_resolution);
    ledcAttachPin(PIN_BUZZER, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);
    
    ESP_LOGI("DRV", "Buzzer初始化完成✅");
}

void buzzer_tone(uint16_t tone, uint8_t note)
{
    ledcWriteTone(PWM_CHANNEL, tone); // 设置频率
    ledcWrite(PWM_CHANNEL, tone / 2);
    vTaskDelay((BEAT_TIME / note) / portTICK_PERIOD_MS);
    ledcWrite(PWM_CHANNEL, 0);
    vTaskDelay(GAP_TIME / portTICK_PERIOD_MS);
}

void buzzer_ready(void)
{
    buzzer_tone(NOTE_C6,4);
    buzzer_tone(NOTE_D6,4);
    buzzer_tone(NOTE_G6,4);
    // ledcWrite(PWM_CHANNEL, 0);
}

void buzzer_beep(int duration_ms)
{
    ledcWriteTone(PWM_CHANNEL, PWM_frequency); // 设置频率
    ledcWrite(PWM_CHANNEL, PWM_frequency / 2);
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    ledcWrite(PWM_CHANNEL, 0);
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
}

void buzzer_info(void)
{
    ledcWriteTone(PWM_CHANNEL, PWM_frequency); // 设置频率
    for (int i = 0; i < 2; i++)
    {
        ledcWrite(PWM_CHANNEL, PWM_frequency / 2);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ledcWrite(PWM_CHANNEL, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void buzzer_warn(void)
{
    ledcWriteTone(PWM_CHANNEL, PWM_frequency); // 设置频率
    ledcWrite(PWM_CHANNEL, PWM_frequency / 2);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ledcWrite(PWM_CHANNEL, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void buzzer_error(void)
{
    ledcWriteTone(PWM_CHANNEL, PWM_frequency); // 设置频率
    ledcWrite(PWM_CHANNEL, PWM_frequency / 2);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ledcWrite(PWM_CHANNEL, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}