#include "key.h"

void KEY_INIT(int num, KEY_typedef *key_typedef, KEY_MAP *key_map)
{
    for (int i = 0; i < num; i++)
    {
        key_typedef[i].key_value = key_map[i].key;
        key_typedef[i].key_index = i;
        key_typedef[i].KEY_STATE = idle;
        key_typedef[i].time = 0;
        key_typedef[i].buffer_index = -1;
    }
}

void key_process(int i, int state, KEY_typedef *key_typedef)
{
    if (state)
    {
        if (key_typedef[i].KEY_STATE == idle)
        {
            key_typedef[i].KEY_STATE = press;
        }
        else if (key_typedef[i].KEY_STATE == press)
        {
            key_typedef[i].KEY_STATE = release;
            // 按键按下 回调函数
            key_handler(i);
        }
    }
    else
    {
        if (key_typedef[i].KEY_STATE == release)
        {
            key_typedef[i].KEY_STATE = idle;
            // 按键释放 回调函数
            key_handler_release(i);
        }
    }
}

int find_buff_emtpy_index(uint8_t *sendBuffer, int key_buffer_size)
{
    uint8_t i;
    int index = -1;

    for (i = 2; i < key_buffer_size; i++) // 循环buff
    {
        if (index == -1)
        {
            if (sendBuffer[i] == 0) // 如果找到一个位置是空的则返回i
            {
                index = i;
                break;
            }
        }
        else
        {
            index = -1;
        }
    }
    return index;
}

void key_buffer_insert(int buffer_index, int i, KEY_MAP *key_value, uint8_t *sendBuffer)
{
    if (buffer_index != -1)
    {
        sendBuffer[buffer_index] = key_value[i].key;
    }
}

void remove_buff(int buff_index, uint8_t *sendBuffer)
{
    sendBuffer[buff_index] = 0x00;
}

void key_scan(KEY_typedef *key_typedef)
{
    uint8_t row, col;
    for (row = 0; row < ROW_NUM; row++)
    {
        for(int i = 0 ; i < ROW_NUM; i++)
        {
            HAL_GPIO_WritePin(Row_Ports, ROW_1_Pin << i, GPIO_PIN_RESET);
        }
        //! 扫描行 单行输出高电平

        // HAL_GPIO_WritePin(Row_Ports, ROW_1_Pin << row, GPIO_PIN_SET);
        switch(row){
            case 0:
                HAL_GPIO_WritePin(Row_Ports, ROW_1_Pin, GPIO_PIN_SET);
                break;
            case 1:
                HAL_GPIO_WritePin(Row_Ports, ROW_2_Pin, GPIO_PIN_SET);
                break;
            case 2:
                HAL_GPIO_WritePin(Row_Ports, ROW_3_Pin, GPIO_PIN_SET);
                break;
            case 3:
                HAL_GPIO_WritePin(Row_Ports, ROW_4_Pin, GPIO_PIN_SET);
                break;
        }
        for (col = 0; col < COL_NUM; col++)
        {
            //! 扫描列 如果该行有按键按下则输出高电平
            if (HAL_GPIO_ReadPin(Col_Ports, COL_1_Pin << col) == GPIO_PIN_SET)
            {
                // todo ....
                key_process(row * COL_NUM + col, 1,key_typedef );
            }else
            {
                key_process(row * COL_NUM + col, 0,key_typedef );
            
            }
        }
        //! 该行扫描完毕 输出低电平
        // HAL_GPIO_WritePin(Row_Ports, ROW_1_Pin << row, GPIO_PIN_RESET);
    }
    
}



