/*
 * encoder.c
 *
 *  Created on: Nov 1, 2026
 *      Author: KinQuy
 */

#include "encoder.h"
#include "tim.h"
#include "main.h"

void encoder_init(void){
	if (HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL) != HAL_OK)
	    Error_Handler();

	if (HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL) != HAL_OK)
	    Error_Handler();

}
