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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void myNecDecodedCallback(uint16_t address, uint8_t cmd) {
	signal_code = cmd;
	signal_flag = 1;
    HAL_Delay(10);
    NEC_Read(&nec);
}

void myNecErrorCallback() {
    HAL_Delay(10);
    NEC_Read(&nec);
}

void myNecRepeatCallback() {
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
	uint8_t speed = 0, motor_flag = 0, steering_flag = 0;
	uint16_t direction = 900;


	motor1.timer_pwm = &htim4;
	motor1.channel_pwm = TIM_CHANNEL_2;
	motor1.pin_a = MOTOR1_A_Pin;
	motor1.GPIO_a = MOTOR1_A_GPIO_Port;
	motor1.pin_b = MOTOR1_B_Pin;
	motor1.GPIO_b = MOTOR1_B_GPIO_Port;

	motor2.timer_pwm = &htim4;
	motor2.channel_pwm = TIM_CHANNEL_3;
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  nec.timerHandle = &htim2;

  nec.timerChannel = TIM_CHANNEL_1;
  nec.timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_1;

  nec.timingBitBoundary = 3000;
  nec.timingAgcBoundary = 12500;
  nec.type = NEC_NOT_EXTENDED;

  nec.NEC_DecodedCallback = myNecDecodedCallback;
  nec.NEC_ErrorCallback = myNecErrorCallback;
  nec.NEC_RepeatCallback = myNecRepeatCallback;

//Rozpoczecie PWM Timera 3
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  MOTOR_Init(&motor1);
  MOTOR_Init(&motor2);

  NEC_Read(&nec);

  MOTOR_SetSpeed(&motor1, 1, 20);
  MOTOR_SetSpeed(&motor2, 0, 20);

//Ustawienie serwomotoru odpowiedzialnego skręcanie na pozycji uznawanej za zero
  set_ang2(900, 0);

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	  HAL_Delay(200);

    if (signal_flag == 1){
    	signal_flag = 0;
    	switch(signal_code){
    	case 9:
    		if(speed < 80){
    			speed +=10;
    			motor_flag = 1;
    		}
    		break;
    	case 21:
    		if (speed > 0){
    			speed -= 10;
    			motor_flag = 1;
    		}
    		break;
    	case 64:
    		if (direction < 1200){
    			direction += 100;
    			steering_flag = 1;
    		}
    		break;
    	case 67:
    		if (direction > 600){
    			direction -=100;
    			steering_flag = 1;
    		}
    		break;
    	case 68:
    		set_ang1(1700, 0);
    		break;
    	case 7:
    		set_ang1(600, 0);
    		break;
    	default:
    		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
    		HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
    		HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
    	}
    }
    if (motor_flag){
    	motor_flag = 0;
    	MOTOR_SetSpeed(&motor1, 1, speed);
    	MOTOR_SetSpeed(&motor2, 0, speed);
    }
    if (steering_flag){
    	steering_flag = 0;
    	set_ang2(direction, 0);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 200;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 10;
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
