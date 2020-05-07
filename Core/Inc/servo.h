/*
 * servo.h
 *
 *  Created on: May 7, 2020
 *
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "tim.h"
#include <stdint.h>


//TIM_HandleTypeDef htim2;


/* konfiguracja timera do generowania PWM */
#define TIM_NO htim2
#define TIM_CH_NO TIM_CHANNEL_1
//#define TIM_CH_NO2 TIM_CHANNEL_2

/* zakresy katowe pracy serwomechanizmu */
#define ANGLE_MIN 0
#define ANGLE_MAX 1800
/* zakres PWM */
//Sprawdzic zakresy
#define PWM_MIN 1100
#define PWM_MAX 2050

#define STEP ((1000 * (PWM_MAX - PWM_MIN)) / (ANGLE_MAX - ANGLE_MIN))

void set_ang(uint16_t ang, uint8_t mode);



#endif /* INC_SERVO_H_ */
