#include <module_button.h>
#include <map_pin.h>
#include <ds_sys.h>

#include <OneButton.h>

OneButton button_L(PIN_BUTTON_L, true);
OneButton button_R(PIN_BUTTON_R, true);

void button_left_single_click_callback(void)
{
    // sys_state.button_state = 1;
    // sys_state.sound_mode = 1;
    ESP_LOGI("DRV", "Left Button单击事件触发✅");
}

void button_left_long_press_callback(void)
{
    // sys_state.button_state = 2;
    // sys_state.sound_mode = 1;
    ESP_LOGI("DRV", "Left Button长按事件触发✅");
}

void button_right_single_click_callback(void)
{
    // sys_state.button_state = 1;
    // sys_state.sound_mode = 1;
    ESP_LOGI("DRV", "Right Button单击事件触发✅");
}

void button_right_long_press_callback(void)
{
    // sys_state.button_state = 2;
    // sys_state.sound_mode = 1;
    ESP_LOGI("DRV", "Right Button长按事件触发✅");
}

void button_init(void)
{
    button_L.setup(PIN_BUTTON_L, INPUT_PULLUP, true);
    // button_L.setup(PIN_BUTTON_L, INPUT_PULLDOWN, false);
    button_L.attachClick([]() { button_left_single_click_callback();});
    button_L.attachLongPressStart([]() { button_left_long_press_callback();});
    button_L.setDebounceMs(50); // 设置消抖时间为50毫秒
    button_L.setClickMs(200); // 设置单击时间为200毫秒
    button_L.setLongPressIntervalMs(1500); // 设置长按时间为1500毫秒

    button_R.setup(PIN_BUTTON_R, INPUT_PULLUP, true);
    // button_R.setup(PIN_BUTTON_R, INPUT_PULLDOWN, false);
    button_R.attachClick([]() { button_right_single_click_callback();});
    button_R.attachLongPressStart([]() { button_right_long_press_callback();});
    button_R.setDebounceMs(50); // 设置消抖时间为50毫秒
    button_R.setClickMs(200); // 设置单击时间为200毫秒
    button_R.setLongPressIntervalMs(1500); // 设置长按时间为1500毫秒
    
    ESP_LOGI("DRV", "Button初始化完成✅");
}


void button_loop(void)
{
    button_L.tick();
    button_R.tick();
}

