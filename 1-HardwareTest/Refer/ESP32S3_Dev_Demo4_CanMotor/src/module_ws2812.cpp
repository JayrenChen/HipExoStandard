#include <Arduino.h>
#include <module_ws2812.h>
#include <Adafruit_NeoPixel.h>
#include <map_pin.h>

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_WS2812, NEO_GRB + NEO_KHZ800);

void rgb_init(void)
{
    pixels.begin(); 
    pixels.setBrightness(50);
    pixels.setPixelColor(0, COLOR_BLACK); pixels.show(); delay(50);

    ESP_LOGI("DRV", "RGB初始化完成✅");
}

void rgb_ready(void)
{
  for(int i=0; i<2; i++) {
    pixels.setPixelColor(0, COLOR_WHITE); pixels.show(); vTaskDelay(500 / portTICK_PERIOD_MS);
    pixels.setPixelColor(0, COLOR_BLACK); pixels.show(); vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void rgb_rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    vTaskDelay(wait / portTICK_PERIOD_MS);
  }
}

void rgb_blink(void)
{
    static int16_t color_val = 0;
    static int16_t color_stp = 5; // 250ms / 50ms = 5
    static int16_t color_dir = 1;

    if(color_dir==1 && 250-color_val<color_stp)   {color_dir=-1; }
    else if(color_dir==-1 && color_val<color_stp) {color_dir= 1; }
    
    color_val += color_dir*color_stp;

    pixels.setPixelColor(0, pixels.Color(color_val,color_val,color_val)); 
    pixels.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}