/*
 * motor.c
 *
 *  Created on: Nov 1, 2026
 *      Author: KinQuy
 */

#include "motor.h"
#include "tim.h"
#include "main.h"

#define MOTOR_LEFT_CHANNEL    TIM_CHANNEL_1
#define MOTOR_RIGHT_CHANNEL   TIM_CHANNEL_2

//Use for only this file
static void set_pwm(uint32_t channel, uint32_t duty){
	if(duty>100) duty = 100;

	uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim3);

	//Bản chất duty cycle là dựa vào số xung trong cái mức cần thiết, ở đây là 1000. Thì nếu duty 20% thì ta đếm 200 lần đầu là 1, 800 còn lại là 0

	uint32_t ccr = (arr+1U)*(duty)/100U;
    __HAL_TIM_SET_COMPARE(&htim3, channel, ccr);
}

void motor_init(void)
{
    if (HAL_TIM_PWM_Start(&htim3, MOTOR_LEFT_CHANNEL) != HAL_OK)
        Error_Handler();

    if (HAL_TIM_PWM_Start(&htim3, MOTOR_RIGHT_CHANNEL) != HAL_OK)
        Error_Handler();

    // Set initial duty to 0%
    set_pwm(MOTOR_LEFT_CHANNEL, 0);
    set_pwm(MOTOR_RIGHT_CHANNEL, 0);
}

void motor_set_left(uint32_t duty){
	set_pwm(MOTOR_LEFT_CHANNEL, duty);
}
void motor_set_right(uint32_t duty){
	set_pwm(MOTOR_RIGHT_CHANNEL, duty);
}

void motor_stop(void){
	motor_set_left(0);
	motor_set_right(0);
}



