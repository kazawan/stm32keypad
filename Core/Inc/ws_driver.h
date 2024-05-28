#ifndef WS_DRIVER_H
#define WS_DRIVER_H
#include "stm32f1xx_hal.h"
#include "main.h"

#define _weak_ __attribute__((weak))

#define PIXEL_NUMS 15 // 灯珠数量
#define PIXEL_BITS_NUMS 24 // 灯珠位数
#define PIXEL_BITS_LENS PIXEL_NUMS * PIXEL_BITS_NUMS // 灯珠总位数

enum
{
    CODE_H = 0XF,
    CODE_L = 0XC
} WS_CODE;


typedef struct
{
    uint32_t color;
    unsigned int timer;
    uint8_t brightness;

}PIXEL_TypeDef;

// 颜色转换
uint32_t ws_color(uint8_t red,uint8_t green,uint8_t blue);
uint32_t ws_color_brighness(uint8_t red,uint8_t green,uint8_t blue,uint16_t brightness);
uint32_t ws_wheel(uint8_t wheelPos);


// set pixel color
void ws_setPixelColor(uint16_t n, uint32_t GRBcolor);

// 初始化 & 显示
void ws_init(void);
void ws_setBrightness(uint8_t brightness);
void ws_clearAll(void);
void ws_pixel_to_buffer();
void ws_show(void);
_weak_ void show_handler(uint8_t *ws_buffer);

// fade in & fade out
void ws_effect_fadeToBlack(uint16_t n, uint8_t wait);


#endif