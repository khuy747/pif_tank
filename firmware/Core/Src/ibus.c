/*
 * ibus.c
 *
 *  Created on: Nov 9, 2026
 *      Author: KinQuy
 */

#include <ibus.h>
#include <stdbool.h>

iBUS_t ibus;

void iBUS_init(UART_HandleTypeDef *huart){
	huart->Instance = USART1;
	huart->Init.BaudRate = 115200;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(huart) != HAL_OK)
	{
	Error_Handler();
	}
}


bool iBUS_checkdata(uint8_t *frame){

    if (frame[0] != 0x20 || frame[1] != 0x40) return false;

    uint16_t checksum = 0xffff;

    for (uint8_t i = 0; i < 30; i++)
    {
        checksum -= frame[i];
    }

    // Lấy checksum do RX gửi
    uint16_t rx_checksum = (uint16_t)frame[30] | ((uint16_t)frame[31] << 8);

    // So sánh
    return checksum == rx_checksum;
}

void iBUS_decode(iBUS_t *ibus, uint8_t* frame){
	if(iBUS_checkdata(frame) == false) return;
	ibus->RH = (frame[2] | frame[3] << 8);
	ibus->RV = (frame[4] | frame[5] << 8);

	ibus->LH = (frame[6] | frame[7] << 8);
	ibus->LV = (frame[8] | frame[9] << 8);

	ibus->SwA = (frame[10] | frame[11] << 8);
	ibus->SwB = (frame[12] | frame[13] << 8);
	ibus->SwC = (frame[14] | frame[15] << 8);
	ibus->SwD = (frame[16] | frame[17] << 8);

	ibus->VrA = (frame[18] | frame[19] << 8);
	ibus->VrB = (frame[20] | frame[21] << 8);
}


void iBUS_parse(uint8_t *frame, uint8_t data){
	static uint8_t index=0;
	if (index == 0)
	    {
	        if (data != 0x20) return;
	    }

	else if (index == 1)
		{
		if (data != 0x40)
		        {
		            index = 0;
		            if (data == 0x20)
		                index = 1;
		            else
		                return;

		            frame[0] = 0x20;
		            return;
		        }
		}

    frame[index] = data;
    index++;

    if (index == 32)
    {
        index = 0;
        if (iBUS_checkdata(frame))
                {
                    iBUS_decode(&ibus, frame);
                }
    }
}
