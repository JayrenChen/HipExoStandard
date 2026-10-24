#include <Arduino.h>
#include <module_ws2812.h>
#include <Adafruit_NeoPixel.h>
#include <map_pin.h>

Adafruit_NeoPixel pixels_L(NUMPIXELS, PIN_WS2812_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_R(NUMPIXELS, PIN_WS2812_R, NEO_GRB + NEO_KHZ800);

void rgb_init(void)
{
    pixels_L.begin();
    pixels_L.setBrightness(50);
    pixels_R.begin();
    pixels_R.setBrightness(50);
    for(int i=0; i<NUMPIXELS; i++) {
        pixels_L.setPixelColor(i, COLOR_WHITE);
        pixels_R.setPixelColor(i, COLOR_WHITE);
    }
    pixels_L.show(); delay(50);
    pixels_R.show(); delay(50);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI("DRV", "RGB初始化完成✅");
}

void rgb_ready(void)
{
    for(int i=0; i<NUMPIXELS; i++) {
        pixels_L.setPixelColor(i, COLOR_GREEN);
        pixels_R.setPixelColor(i, COLOR_GREEN);
    }
    pixels_L.show(); delay(50);
    pixels_R.show(); delay(50);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void rgb_blink(void)
{
  static int8_t black_inx = 0;
  for(int i=0; i<NUMPIXELS; i++) {
    if(i==black_inx) {
      pixels_L.setPixelColor(i, COLOR_BLACK);
      pixels_R.setPixelColor(i, COLOR_BLACK);
    }
    else {
      pixels_L.setPixelColor(i, COLOR_WHITE);
      pixels_R.setPixelColor(i, COLOR_WHITE);
    }
  }
  black_inx = (black_inx + 1) % NUMPIXELS;
  pixels_L.show();
  pixels_R.show();
}

void rgb_rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i<pixels_L.numPixels(); i++) {
      pixels_L.setPixelColor(i, Wheel(((i * 256 / pixels_L.numPixels()) + j) & 255));
    }
    pixels_L.show();
    vTaskDelay(wait / portTICK_PERIOD_MS);
  }
}

// void rgb_blink(void)
// {
//     static int16_t color_val = 0;
//     static int16_t color_stp = 5; // 250ms / 50ms = 5
//     static int16_t color_dir = 1;

//     if(color_dir==1 && 250-color_val<color_stp)   {color_dir=-1; }
//     else if(color_dir==-1 && color_val<color_stp) {color_dir= 1; }
    
//     color_val += color_dir*color_stp;

//     pixels_L.setPixelColor(0, pixels_L.Color(color_val,color_val,color_val)); 
//     pixels_L.show();
// }

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels_L.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels_L.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels_L.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}