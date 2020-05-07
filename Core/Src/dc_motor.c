/*
 * dc_motor.c
 *
 *  Created on: Apr 30, 2020
 *      Author: researcher
 */
#include "dc_motor.h"




void MOTOR_Init(MOTOR_HandleTypeDef *hmotor){
	HAL_TIM_PWM_Start(hmotor->timer_pwm, hmotor->channel_pwm);
}
//direction 0=left 1=right
void MOTOR_SetSpeed(MOTOR_HandleTypeDef *hmotor, uint8_t direction, uint8_t duty){
	if (direction == 1){
		HAL_GPIO_WritePin(hmotor->GPIO_a , hmotor->pin_a, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hmotor->GPIO_b , hmotor->pin_b, GPIO_PIN_RESET);
	}
	else if (direction == 0){
		HAL_GPIO_WritePin(hmotor->GPIO_a , hmotor->pin_a, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(hmotor->GPIO_b , hmotor->pin_b, GPIO_PIN_SET);
	}
	__HAL_TIM_SET_COMPARE(hmotor->timer_pwm, hmotor->channel_pwm, duty*10);
}

void MOTOR_Break(MOTOR_HandleTypeDef *hmotor){
	HAL_GPIO_WritePin(hmotor->GPIO_a , hmotor->pin_a, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(hmotor->GPIO_b , hmotor->pin_b, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(hmotor->timer_pwm, hmotor->channel_pwm, 0);
}

void MOTOR_Stop(MOTOR_HandleTypeDef *hmotor){
	__HAL_TIM_SET_COMPARE(hmotor->timer_pwm, hmotor->channel_pwm, 0);
}
