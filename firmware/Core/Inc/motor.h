/*
 * motor.h
 *
 *  Created on: Nov 1, 2026
 *      Author: KinQuy
 */

//Purpose: Control DC Motor speed

//Specs: ARR=999, Need function to control TIM3 to ouput PWM Frequency and some GPIO to control the direction
//Archi: Từ bánh mình cần --> Điều khiển PWM --> Xuất đúng Channel

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdint.h>

void motor_init(void);

void motor_set_left(uint32_t duty);
void motor_set_right(uint32_t duty);

void motor_stop(void);

#endif /* INC_MOTOR_H_ */
