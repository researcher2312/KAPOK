/*
 * servo.c
 *
 *  Created on: May 7, 2020
 *      Author: nowik
 */


#include "servo.h"
#include "main.h"

/*
 * ang - kat obrotu walu serwomechanizmu
 * mode - tryb obrotu zgodnie/przeciwnie do wskazowek zegara
 */
void set_ang1(uint16_t ang, uint8_t mode)
{
	uint16_t val;

	if(ang > ANGLE_MAX)
	{
		ang = ANGLE_MAX;
	}
	else if (ang < ANGLE_MIN)
	{
		ang = ANGLE_MIN;
	}

	if(mode)
	{
		val = PWM_MIN + ((ang - ANGLE_MIN) * STEP) / 1000;
	}
	else
	{
		val = PWM_MAX - ((ang - ANGLE_MIN) * STEP) / 1000;
	}

	__HAL_TIM_SET_COMPARE(&TIM_NO, TIM_CH_NO, val);
}

//Domyślnie najlepiej angle = {600-1200}
void set_ang2(uint16_t ang, uint8_t mode)
{
	uint16_t val;

	if(ang > ANGLE_MAX)
	{
		ang = ANGLE_MAX;
	}
	else if (ang < ANGLE_MIN)
	{
		ang = ANGLE_MIN;
	}

	if(mode)
	{
		val = PWM_MIN + ((ang - ANGLE_MIN) * STEP) / 1000;
	}
	else
	{
		val = PWM_MAX - ((ang - ANGLE_MIN) * STEP) / 1000;
	}

	__HAL_TIM_SET_COMPARE(&TIM_NO, TIM_CH_NO2, val);
}


