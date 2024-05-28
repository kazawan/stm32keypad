#include "ws_driver.h"

uint8_t PIXEL_BUFFER[PIXEL_NUMS][PIXEL_BITS_NUMS] = {0};
PIXEL_TypeDef PIXEL[PIXEL_NUMS] = {0};
uint8_t All_brightness = 255;


void ws_init(void)
{
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
        PIXEL[i].color = 0x000000;
        PIXEL[i].brightness = 255;
        PIXEL[i].timer = 0;
    }
}


uint32_t ws_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return green << 16 | red << 8 | blue;
}

uint32_t ws_color_brighness(uint8_t red, uint8_t green, uint8_t blue, uint16_t brightness)
{
    uint8_t r, g, b;
    r = red * brightness / 255;
    g = green * brightness / 255;
    b = blue * brightness / 255;
    return ws_color(r, g, b);
}


void ws_setPixelColor(uint16_t n, uint32_t GRBcolor)
{   
    if(n < 0 || n > PIXEL_NUMS) return;
 
    PIXEL[n].color = GRBcolor;
}

uint32_t ws_wheel(uint8_t wheelPos)
{
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85)
    {
        return ws_color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170)
    {
        wheelPos -= 85;
        return ws_color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return ws_color(wheelPos * 3, 255 - wheelPos * 3, 0);
}


void ws_setBrightness(uint8_t brightness)
{

    if (brightness > 255)
    {
        brightness = 255;
    }
    if (brightness < 0)
    {
        brightness = 0;
    }
    All_brightness = brightness;
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
        PIXEL[i].brightness = brightness;
    }
}

void ws_show()
{
    uint8_t buff[PIXEL_NUMS * PIXEL_BITS_NUMS] = {0};
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
        for (int j = 0; j < PIXEL_BITS_NUMS; j++)
        {
            buff[i * PIXEL_BITS_NUMS + j] = PIXEL_BUFFER[i][j];
        }
    }
    show_handler(buff);
}


void ws_clearAll(void)
{
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
        for (int j = 0; j < PIXEL_BITS_NUMS; j++)
        {
            PIXEL_BUFFER[i][j] = CODE_L;
        }
    }
}

void ws_effect_fadeToBlack(uint16_t n, uint8_t wait)
{

    if (HAL_GetTick() - PIXEL[n].timer > wait)
    {
        if (PIXEL[n].brightness == 0)
        {
            return;
        }
        if (PIXEL[n].brightness > 0)
        {
            PIXEL[n].brightness -= 1;
        }
        if (PIXEL[n].brightness < 0)
        {
            PIXEL[n].brightness = 0;
        }

        PIXEL[n].timer = HAL_GetTick();
    }
}




void ws_pixel_to_buffer()
{
    uint8_t i, j;

    for (i = 0; i < PIXEL_NUMS; i++)
    {
        uint8_t g = (PIXEL[i].color >> 16) & 0XFF * PIXEL[i].brightness / 255;
        uint8_t r = (PIXEL[i].color >> 8) & 0XFF * PIXEL[i].brightness / 255;
        uint8_t b = PIXEL[i].color & 0XFF * PIXEL[i].brightness / 255;
        for (j = 0; j < PIXEL_BITS_NUMS; j++)
        {
            PIXEL_BUFFER[i][j] = (((ws_color(r, g, b) << j) & 0X800000) ? CODE_H : CODE_L);
        }
    }
}
