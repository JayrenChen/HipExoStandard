#include <module_button.h>
#include <map_pin.h>
#include <ds_sys.h>

#include <OneButton.h>

OneButton button(PIN_BUTTON, true);

void button_single_click_callback(void)
{
    // sys_state.button_state = 1;
    sys_state.sound_mode = 1;
    ESP_LOGI("DRV", "Button单击事件触发✅");
}

void button_init(void)
{
    button.setup(PIN_BUTTON, INPUT_PULLUP, true);
    button.attachClick([]() { button_single_click_callback();});
    ESP_LOGI("DRV", "Button初始化完成✅");
}


void button_loop(void)
{
    button.tick();
}

