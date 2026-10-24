#ifndef __MODULE_RGB_H
#define __MODULE_RGB_H
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define PIN        48 //  根据需要修改  38 16 8
#define NUMPIXELS  1
#define PIN4       43 //  根据需要修改  38 16 8
#define NUMPIXELS4  4
#define NORM_BLINK_TIME  50
#define FAST_BLINK_TIME  20W

#define NUM_PIX_LINE  15
#define PIN_LINE      6 //  根据需要修改  38 16 8

#define COLOR_RED 0x00FF0000
#define COLOR_BLUE 0x000000FF
#define COLOR_GREEN 0x0000FF00
#define COLOR_BLACK 0x00000000
#define COLOR_ORANGE 0x00FF6F00
#define COLOR_WHITE 0x00FFFFFF
#define COLOR_WHITE_BB 0x00999999
#define COLOR_WHITE_77 0x00555555
#define COLOR_WHITE_33 0x00111111
#define COLOR_PINK 0x00880E4F

// #define Boot_Mode_White_Blink
#define Boot_Mode_White_Float
// #define Boot_Mode_White_Center
// #define Boot_Mode_White_Center_Breathe

enum RGB_Mode_TypeDef
{
    RGB_Mode_IMU_Cycle = 0,
    RGB_Mode_Rainbow_Cycle = 1,
    RGB_Mode_Breathe_Cycle = 2,
    RGB_Mode_Light_ON = 3,
    RGB_Mode_Light_OFF = 4,
    RGB_Mode_Charge_Cycle = 5,
};

void Setup_RGB(void);
void ReSet_RGB(void);
void RGB_Blink_Blue_100ms(void);
void RGB_Blink_Green_1000ms(void);
void RGB_Blink_Blue_1000ms(void);
void RGB_Blink_Orange_100ms(void);
void RGB_Blink_Orange_1000ms(void);
void RGB_Blink_Orange_Block(void);
void RGB_Blink_Pink_100ms(void);
void RGB_Blink_Pink_1000ms(void);
void RGB_Blink_Pink_Block(void);
void Setup_RGB_LINE(void);
void Process_RGB(void);
void Rainbow_Cycle(void);
void Breathe_Cycle(void);
void Light_ON(void);
void Light_OFF(void);
void Charge_Cycle(void);

#endif