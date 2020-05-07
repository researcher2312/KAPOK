/*
 * dc.h
 *
 *  Created on: Apr 30, 2020
 *      Author: researcher
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#include "tim.h"
#include "gpio.h"


typedef struct{
	TIM_HandleTypeDef *timer_pwm;
	uint32_t channel_pwm;
	uint32_t pin_a;
	GPIO_TypeDef* GPIO_a;
	uint32_t pin_b;
	GPIO_TypeDef* GPIO_b;
}MOTOR_HandleTypeDef;

void MOTOR_Init(MOTOR_HandleTypeDef *hmotor);
void MOTOR_SetSpeed(MOTOR_HandleTypeDef *hmotor, uint8_t direction, uint8_t duty);
void MOTOR_Break(MOTOR_HandleTypeDef *hmotor);
void MOTOR_Stop(MOTOR_HandleTypeDef *hmotor);

#endif /* INC_MOTOR_H_ */
