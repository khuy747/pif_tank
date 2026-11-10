/*
 * ibus.h
 *
 *  Created on: Nov 9, 2026
 *      Author: KinQuy
 */

#ifndef INC_IBUS_H_
#define INC_IBUS_H_

#include <stdbool.h>
#include <stdint.h>
#include <main.h>

//iBUS: Init --> Read (need sth to store) (Can do this in MAIN ussing callback) --> Check --> Decode.

typedef struct
{
    uint16_t RH;
    uint16_t RV;
    uint16_t LV;
    uint16_t LH;

    uint16_t SwA;
    uint16_t SwB;
    uint16_t SwC;
    uint16_t SwD;

    uint16_t VrA;
    uint16_t VrB;

} iBUS_t;


void iBUS_init(UART_HandleTypeDef *huart);

//void iBUS_readdata(uint8_t *frame);

bool iBUS_checkdata(uint8_t *frame);

void iBUS_decode(iBUS_t *ibus, uint8_t* frame);

void iBUS_parse(uint8_t *frame, uint8_t data);

#endif /* INC_IBUS_H_ */
