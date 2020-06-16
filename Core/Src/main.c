/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "tim.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "NEC_Decode.h"
#include "dc_motor.h"
#include "servo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
NEC nec;

MOTOR_HandleTypeDef motor1;
MOTOR_HandleTypeDef motor2;

volatile uint8_t signal_flag = 0;
volatile uint8_t signal_code;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void myNecDecodedCallback(uint16_t address, uint8_t cmd) {
//	printf("Adres: %d komenda: %d\r\n", address, cmd);
	signal_code = cmd;
	signal_flag = 1;
    HAL_Delay(10);
    NEC_Read(&nec);
}

void myNecErrorCallback() {
//    printf("Error\r\n");
    HAL_Delay(10);
    NEC_Read(&nec);
}

void myNecRepeatCallback() {
//    printf("Repeat\r\n");
	signal_flag = 1;
    HAL_Delay(10);
    NEC_Read(&nec);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        NEC_TIM_IC_CaptureCallback(&nec);
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
	uint8_t speed = 0, motor_flag = 0, steering_flag = 0, arm_flag = 0;;
	uint16_t direction = 900, position = 0;


	motor1.timer_pwm = &htim2;
	motor1.channel_pwm = TIM_CHANNEL_1;
	motor1.pin_a = MOTOR1_A_Pin;
	motor1.GPIO_a = MOTOR1_A_GPIO_Port;
	motor1.pin_b = MOTOR1_B_Pin;
	motor1.GPIO_b = MOTOR1_B_GPIO_Port;

	motor2.timer_pwm = &htim2;
	motor2.channel_pwm = TIM_CHANNEL_2;
	motor2.pin_a = MOTOR2_A_Pin;
	motor2.GPIO_a = MOTOR2_A_GPIO_Port;
	motor2.pin_b = MOTOR2_B_Pin;
	motor2.GPIO_b = MOTOR2_B_GPIO_Port;
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
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  nec.timerHandle = &htim2;

  nec.timerChannel = TIM_CHANNEL_1;
  nec.timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_1;

  nec.timingBitBoundary = 1680;
  nec.timingAgcBoundary = 12500;
  nec.type = NEC_EXTENDED;

  nec.NEC_DecodedCallback = myNecDecodedCallback;
  nec.NEC_ErrorCallback = myNecErrorCallback;
  nec.NEC_RepeatCallback = myNecRepeatCallback;

  MOTOR_Init(&motor1);
  MOTOR_Init(&motor2);

  NEC_Read(&nec);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 //set_ang(100,0);
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
    if (signal_flag == 1){
    	signal_flag = 0;
    	switch(signal_code){
    	case 9:
    		if(speed < 100){
    			speed +=1;
    			motor_flag = 1;
    		}
    		break;
    	case 21:
    		if (speed > 0){
    			speed -= 1;
    			motor_flag = 1;
    		}
    		break;
    	case 64:
    		if (direction > 0){
    			direction -= 10;
    			steering_flag = 1;
    		}
    		break;
    	case 67:
    		if (direction < 1800){
    			direction +=10;
    			steering_flag = 1;
    		}
    		break;
    	case 68:
    		if (position < 1800){
    			position += 10;
    			arm_flag = 1;
    		}
    		break;
    	case 7:
    		if (position > 0){
    			position -= 10;
    			arm_flag = 1;
    		}
    		break;
    	}
    }
    if (motor_flag){
    	motor_flag = 0;
    	MOTOR_SetSpeed(&motor1, 1, speed);
    	MOTOR_SetSpeed(&motor2, 1, speed);
    }
    if (steering_flag){
    	steering_flag = 0;
    }
    if (arm_flag){
    	arm_flag = 0;
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 200;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 5;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
