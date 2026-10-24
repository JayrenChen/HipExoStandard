#ifndef __MODULE_WS2812_
#define __MODULE_WS2812_

#define NUMPIXELS  5
#define NORM_BLINK_TIME  5
#define FAST_BLINK_TIME  2

#define COLOR_RED 0x00FF0000
#define COLOR_BLUE 0x000000FF
#define COLOR_GREEN 0x0000FF00
#define COLOR_BLACK 0x00000000
#define COLOR_ORANGE 0x00FF6F00
#define COLOR_WHITE 0x00FFFFFF
#define COLOR_PURPLE 0x00880E4F

void rgb_init(void);
void rgb_ready(void);
void rgb_blink(void);
void rgb_rainbow(uint8_t wait);
uint32_t Wheel(uint8_t WheelPos);

#endif