/**
 * @file module_rgb.cpp
 * @author JYChen
 * @brief WS2812-RGB灯控制
 * @version 0.1
 * @date 2025-03-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <module_rgb.h>
#include <module_wifi.h>

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixel_line(NUM_PIX_LINE, PIN_LINE, NEO_GRB + NEO_KHZ800);

extern float BatVol;  // 电池电压
extern float HostVol; // 电池电压

/**
 * @brief 初始化板载WS2812, PIn = 48
 * 
 */
void Setup_RGB(void)
{
    pixels.begin();
    pixels.setBrightness(10);
    pixels.setPixelColor(0, COLOR_WHITE);pixels.show();delay(500);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();delay(500);
    pixels.setPixelColor(0, COLOR_WHITE);pixels.show();delay(500);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();delay(500);

}

/**
 * @brief 初始化RGB灯带
 */
void Setup_RGB_LINE(void)
{
    pixel_line.begin();
    pixel_line.setBrightness(50);
    #ifdef Boot_Mode_White_Blink
        for (int8_t i = 0; i < NUM_PIX_LINE; i++)
            pixel_line.setPixelColor(i, COLOR_WHITE);
        pixel_line.show();delay(500);
        for (int8_t i = 0; i < NUM_PIX_LINE; i++)
            pixel_line.setPixelColor(i, COLOR_BLACK);
        pixel_line.show();delay(500);
        for (int8_t i = 0; i < NUM_PIX_LINE; i++)
            pixel_line.setPixelColor(i, COLOR_WHITE);
        pixel_line.show();delay(500);
        for (int8_t i = 0; i < NUM_PIX_LINE; i++)
            pixel_line.setPixelColor(i, COLOR_BLACK);
        pixel_line.show();delay(500);
    #endif
    #ifdef Boot_Mode_White_Float
        for(int8_t i=0; i<NUM_PIX_LINE/3; i+=1)
        {
            for(int8_t j=0; j<NUM_PIX_LINE; j++)
                if(j/3==i)
                    pixel_line.setPixelColor(j, COLOR_WHITE);
                else
                    pixel_line.setPixelColor(j, COLOR_BLACK);
            pixel_line.show(); delay(100);
        }
        for(int8_t i=NUM_PIX_LINE/3-1; i>=0; i-=1)
        {
            for(int8_t j=0; j<NUM_PIX_LINE; j++)
                if(j/3==i)
                    pixel_line.setPixelColor(j, COLOR_WHITE);
                else
                    pixel_line.setPixelColor(j, COLOR_BLACK);
            pixel_line.show(); delay(100);
        }
        for(int8_t j=0; j<NUM_PIX_LINE; j++)
            pixel_line.setPixelColor(j, COLOR_BLACK);
        pixel_line.setBrightness(100);
        pixel_line.show(); delay(100);
    #endif
    #ifdef Boot_Mode_White_Center
        for(int8_t cnt=0; cnt<3; cnt++)
        {
            uint8_t rgb_list[4][NUM_PIX_LINE] = {{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                                                 {0,0,0,0,1,0,1,0,1,0,0,0,0,0,0},
                                                 {0,1,0,1,0,1,0,0,0,1,0,1,0,1,0},
                                                 {1,0,1,0,0,0,0,0,0,0,0,0,1,0,1}};
            for(int8_t i=0; i<4; i++)
            {
                for(int8_t j=0; j<NUM_PIX_LINE; j++)
                    if(rgb_list[i][j]==1)
                        pixel_line.setPixelColor(j, COLOR_WHITE);
                    else
                        pixel_line.setPixelColor(j, COLOR_BLACK);
                pixel_line.show(); delay(100);
            }
        }
        for(int8_t j=0; j<NUM_PIX_LINE; j++)
            pixel_line.setPixelColor(j, COLOR_BLACK);
        pixel_line.show(); delay(100);
    #endif
    #ifdef Boot_Mode_White_Center_Breathe
        uint32_t rgb_list[NUM_PIX_LINE] = {COLOR_WHITE_33,COLOR_WHITE_77,COLOR_WHITE_33,
                                           COLOR_WHITE_77,COLOR_WHITE_BB,COLOR_WHITE_77,
                                           COLOR_WHITE_BB,COLOR_WHITE,   COLOR_WHITE_BB,
                                           COLOR_WHITE_77,COLOR_WHITE_BB,COLOR_WHITE_77,
                                           COLOR_WHITE_33,COLOR_WHITE_77,COLOR_WHITE_33};
        for(size_t cnt=0; cnt<1; cnt++)
        {
            for(int16_t cnt=0; cnt<255; cnt+=16)
            {
                for(int8_t j=0; j<NUM_PIX_LINE; j++)
                    pixel_line.setPixelColor(j, rgb_list[j]);
                pixel_line.setBrightness(cnt);
                pixel_line.show(); delay(50);
            }
            for(int16_t cnt=255; cnt>0; cnt-=16)
            {
                for(int8_t j=0; j<NUM_PIX_LINE; j++)
                    pixel_line.setPixelColor(j, rgb_list[j]);
                pixel_line.setBrightness(cnt);
                pixel_line.show(); delay(50);
            }
        }
        pixel_line.setBrightness(50);
        for(int8_t j=0; j<NUM_PIX_LINE; j++)
            pixel_line.setPixelColor(j, COLOR_BLACK);
        pixel_line.show(); delay(100);
    #endif

}


void Process_RGB(void)
{
    for(int8_t i = 0; i < NUM_IMU; i++)
    {
        if(IMU_FPS[i] > 90)
            pixel_line.setPixelColor(i, COLOR_BLUE);
        else if(IMU_FPS[i] > 50)
            pixel_line.setPixelColor(i, COLOR_GREEN);
        else if(IMU_FPS[i] > 20)
            pixel_line.setPixelColor(i, COLOR_ORANGE);
        else if(IMU_FPS[i] > 0)
            pixel_line.setPixelColor(i, COLOR_RED);
        else
            pixel_line.setPixelColor(i, COLOR_BLACK);
    }
    pixel_line.show();
}

void Rainbow_Cycle(void)
{
    static uint16_t hue = 0;
    static int8_t sat = 255;
    static int8_t val = 255;

    hue += 1024;

    for(int i=0;i<5;i++) pixel_line.setPixelColor(3*i,       pixel_line.ColorHSV(hue+      (float)(i)*65536.0/(NUM_PIX_LINE),sat,val));
    for(int i=0;i<5;i++) pixel_line.setPixelColor(3*(4-i)+1, pixel_line.ColorHSV(hue+21845+(float)(i)*65536.0/(NUM_PIX_LINE),sat,val));
    for(int i=0;i<5;i++) pixel_line.setPixelColor(3*i+2,     pixel_line.ColorHSV(hue+43690+(float)(i)*65536.0/(NUM_PIX_LINE),sat,val));
    pixel_line.show(); 
}

void Breathe_Cycle(void)
{
    static uint16_t bright = 0;
    static int8_t dir = 1;
    static uint32_t rgb_list[NUM_PIX_LINE] = {COLOR_WHITE_33,COLOR_WHITE_77,COLOR_WHITE_33,
                                           COLOR_WHITE_77,COLOR_WHITE_BB,COLOR_WHITE_77,
                                           COLOR_WHITE_BB,COLOR_WHITE,   COLOR_WHITE_BB,
                                           COLOR_WHITE_77,COLOR_WHITE_BB,COLOR_WHITE_77,
                                           COLOR_WHITE_33,COLOR_WHITE_77,COLOR_WHITE_33};
    if(bright==0) dir = 1;
    if(bright>=252) dir = -1;
    bright += dir*4;
    for(int8_t j=0; j<NUM_PIX_LINE; j++)
        pixel_line.setPixelColor(j, rgb_list[j]);
    pixel_line.setBrightness(bright);
    pixel_line.show();
}

void Charge_Cycle(void)
{
    const  uint16_t bright = 5;
    static uint16_t blink_cnt = 0;
    uint16_t n_led = 0;

    n_led = (floor)( (BatVol - 3.3) / (4.2 - 3.3) * NUM_PIX_LINE );
    blink_cnt++;

    if(blink_cnt >= 100)
        blink_cnt = 0;
    else if(blink_cnt >= 50)
        n_led += 1;
    
    for(int8_t j=0; j<NUM_PIX_LINE; j++)
        if(j < n_led)
            pixel_line.setPixelColor(j, COLOR_WHITE);
        else
            pixel_line.setPixelColor(j, COLOR_BLACK);
    pixel_line.setBrightness(bright);
    pixel_line.show();
}

void Light_ON(void)
{
    pixel_line.setBrightness(255);
    for(int8_t j=0; j<NUM_PIX_LINE; j++)
        pixel_line.setPixelColor(j, COLOR_WHITE);
    pixel_line.show();
}

void Light_OFF(void)
{
    for(int8_t j=0; j<NUM_PIX_LINE; j++)
        pixel_line.setPixelColor(j, COLOR_BLACK);
    pixel_line.setBrightness(50);
    pixel_line.show();
}


/**
 * @brief 设置RGB灯颜色
 * 
 * @param bright [0, 255]
 */
void Set_RGB_Bright(uint8_t bright)
{
    pixels.setBrightness(bright);
}

/**
 * @brief 关闭RGB灯(黑色)
 * 
 */
void ReSet_RGB(void)
{
    pixels.setPixelColor(0, COLOR_BLACK);
    pixels.show();
}

/**
 * @brief RGB蓝色快闪
 * 
 */
void RGB_Blink_Blue_100ms(void)
{
    pixels.setPixelColor(0, COLOR_BLUE);pixels.show();
    delay(100);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(100);
}

/**
 * @brief RGB蓝色长闪
 * 
 */
void RGB_Blink_Blue_1000ms(void)
{
    pixels.setPixelColor(0, COLOR_BLUE);pixels.show();
    delay(1000);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(1000);
}

/**
 * @brief RGB橙色阻塞
 * 
 */
void RGB_Blink_Orange_Block(void)
{
    while(1)
    {
        pixels.setPixelColor(0, COLOR_ORANGE);pixels.show();
        delay(1000);
        // pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
        // delay(100);
    }
}

/**
 * @brief RGB橙色快闪
 * 
 */
void RGB_Blink_Orange_100ms(void)
{
    pixels.setPixelColor(0, COLOR_ORANGE);pixels.show();
    delay(100);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(100);
}

/**
 * @brief RGB橙色长闪
 * 
 */
void RGB_Blink_Orange_1000ms(void)
{
    pixels.setPixelColor(0, COLOR_ORANGE);pixels.show();
    delay(1000);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(1000);
}

/**
 * @brief RGB粉色快闪
 * 
 */
void RGB_Blink_Pink_100ms(void)
{
    pixels.setPixelColor(0, COLOR_PINK);pixels.show();
    delay(100);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(100);
}

/**
 * @brief RGB粉色长闪
 * 
 */
void RGB_Blink_Pink_1000ms(void)
{
    pixels.setPixelColor(0, COLOR_PINK);pixels.show();
    delay(1000);
    pixels.setPixelColor(0, COLOR_BLACK);pixels.show();
    delay(1000);
}

/**
 * @brief RGB粉色阻塞
 * 
 */
void RGB_Blink_Pink_Block(void)
{
    while(1)
    {
        pixels.setPixelColor(0, COLOR_PINK);pixels.show();
        delay(1000);
    }
}