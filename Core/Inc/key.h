#ifndef KEY_H
#define KEY_H
#include "stm32f1xx_hal.h"
#include "main.h"
#include "usb_key_define.h"
#define _weak___ __attribute__((weak))

//! OUTPUT  到 INPUT 在开关input那边放一个二极管 1n4148
//! Row 使用输出OUTPUT PP 
//! Col 使用输入 INPUT PULL_DOWN
//todo     c1   c2   c3   c4
//todo r1 1 -> 2 -> 3 -> 4
//todo 




#define ROW_NUM 4
#define COL_NUM 4

typedef struct
{
    char key;
} KEY_MAP;

enum 
{
    idle,
    press,
    release,
}KEY_STATE;

typedef struct
{
    char *key_value;
    int key_index;
    int KEY_STATE;
    unsigned long long time;
    // unsigned long debounce_time;
    int buffer_index;
}KEY_typedef;



// output
#define Row_Ports GPIOB
#define ROW_1_Pin GPIO_PIN_8
#define ROW_2_Pin GPIO_PIN_9
#define ROW_3_Pin GPIO_PIN_10
#define ROW_4_Pin GPIO_PIN_11

// input
#define Col_Ports GPIOB
#define COL_1_Pin GPIO_PIN_4
#define COL_2_Pin GPIO_PIN_5
#define COL_3_Pin GPIO_PIN_6
#define COL_4_Pin GPIO_PIN_7





void KEY_INIT(int num, KEY_typedef *key_typedef, KEY_MAP *key_map);
void key_process(int i ,int state,KEY_typedef *key_typedef);
int find_buff_emtpy_index(uint8_t *sendBuffer, int key_buffer_size);
void key_buffer_insert(int buffer_index, int i, KEY_MAP *key_value, uint8_t *sendBuffer);
void remove_buff(int buff_index, uint8_t *sendBuffer);


_weak___ void key_handler(int i);
_weak___ void key_handler_release(int i);

void key_scan(KEY_typedef *key_typedef);

#endif