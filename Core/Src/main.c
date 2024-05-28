/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_hid.h"
#include "key.h"
#include "ws_driver.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY_NUM 15
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//! [code_main]

//! ws2812b
extern PIXEL_TypeDef PIXEL[PIXEL_NUMS];

void show_handler(uint8_t *ws_buffer)
{
  HAL_SPI_Transmit_DMA(&hspi1, ws_buffer, PIXEL_NUMS * PIXEL_BITS_NUMS);
  HAL_Delay(1);
}

//! usb
extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct
{
  uint8_t MODIFIER;
  uint8_t RESERVED;
  uint8_t KEYCODE1;
  uint8_t KEYCODE2;
  uint8_t KEYCODE3;
  uint8_t KEYCODE4;
  uint8_t KEYCODE5;
  uint8_t KEYCODE6;
} keyboardHID_t;

keyboardHID_t keyboardHID = {0, 0, 0, 0, 0, 0, 0, 0};

int KEY_LAYER = 0;
uint8_t modify_buff = 0x00;

KEY_MAP key_map[KEY_NUM] = {
    {keypad_7}, {keypad_8}, {keypad_9}, {keypad_minus}, {keypad_4}, {keypad_5}, {keypad_6}, {keypad_plus}, {keypad_1}, {keypad_2}, {keypad_3}, {keypad_enter}, {keypad_dot}, {keypad_0}, {FN_KEY}};

KEY_MAP key_map_layer1[KEY_NUM] = {
    {None_key}, {None_key}, {keypad_Delete}, {keypad_divide}, {None_key}, {None_key}, {None_key}, {keypad_multiply}, {key_a}, {key_b}, {None_key}, {keypad_enter}, {RIGHT_CTRL_KEY}, {RIGHT_SHIFT_KEY}, {FN_KEY}};

KEY_typedef KEYPAD[KEY_NUM];

void key_handler(int i)
{
  if (i == 14)
  {
    KEY_LAYER = 1;
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
      ws_setPixelColor(i, ws_color(0, 255, 0));
    }
  }
  else if (i == 12 && KEY_LAYER == 1)
  {
    modify_buff |= 0x10;
    keyboardHID.MODIFIER = modify_buff;
  }
  else if (i == 10 && KEY_LAYER == 1)
  {
    modify_buff |= 0x80;
    keyboardHID.MODIFIER = modify_buff;
  }
  else if (i == 13 && KEY_LAYER == 1)
  {
    modify_buff |= 0x20;
    keyboardHID.MODIFIER = modify_buff;
  }
  else
  {
    KEYPAD[i].buffer_index = find_buff_emtpy_index(&keyboardHID, 8);

    if (KEY_LAYER == 0)
    {
      if (KEYPAD[i].buffer_index != -1)
      {
        key_buffer_insert(KEYPAD[i].buffer_index, i, &key_map, &keyboardHID);
      }
    }
    else if (KEY_LAYER == 1)
    {

      if (KEYPAD[i].buffer_index != -1)
      {
        key_buffer_insert(KEYPAD[i].buffer_index, i, &key_map_layer1, &keyboardHID);
      }
    }
  }
}

void key_handler_release(int i)
{
  if (i == 14)
  {
    KEY_LAYER = 0;
    for (int i = 0; i < PIXEL_NUMS; i++)
    {
      ws_setPixelColor(i, ws_color(255, 0, 255));
    }
  }
  else if (i == 10 && KEY_LAYER == 1)
  {
    modify_buff &= 0x7F;
    keyboardHID.MODIFIER = modify_buff;
  }
  else if (i == 12 && KEY_LAYER == 1)
  {
    modify_buff &= 0xEF;
    keyboardHID.MODIFIER = modify_buff;
  }
  else if (i == 13 && KEY_LAYER == 1)
  {
    modify_buff &= 0xDF;
    keyboardHID.MODIFIER = modify_buff;
  }
  else
  {
    remove_buff(KEYPAD[i].buffer_index, &keyboardHID);
    KEYPAD[i].buffer_index = -1;
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  KEY_INIT(KEY_NUM, KEYPAD, key_map);

  ws_clearAll();
  ws_show();
  ws_setBrightness(128);
  for (int i = 0; i < PIXEL_NUMS; i++)
  {
    ws_setPixelColor(i, ws_color(255, 0, 255));
  }
  ws_pixel_to_buffer();
  ws_show();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&keyboardHID, sizeof(keyboardHID));

    key_scan(&KEYPAD);
    ws_pixel_to_buffer();
    ws_show();
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
