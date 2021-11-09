#ifndef UART_H_
#define UART_H_
#include "MKL25Z4.h" 
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "init.h"

#define UP_BUTTON_PRESSED           0x01
#define DOWN_BUTTON_PRESSED         0x02
#define LEFT_BUTTON_PRESSED         0x03
#define RIGHT_BUTTON_PRESSED        0x04

#define RIGHT_FORWARD_BUTTON_PRESSED        0x05
#define RIGHT_REVERSE_BUTTON_PRESSED        0x06
#define LEFT_FORWARD_BUTTON_PRESSED         0x07
#define LEFT_REVERSE_BUTTON_PRESSED         0x08

#define ALL_BUTTON_RELEASED         0x09
#define AUTO         0x10

#define CONNECT 0x00
#define THE_END 0xFF
#define UNIDENTIFIED         0x16

osMessageQueueId_t uartMsg;


/* Interupt for capturing serial data */
void UART2_IRQHandler(void) {
	uint8_t rxData;
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		// Received data from ESP32
			rxData = UART2->D;
			osMessageQueuePut(uartMsg, &rxData, NULL, 0);
	}   
	//Clear INT Flag
	PORTE->ISFR |= MASK(UART_RX_PORTE23);
}

#endif
