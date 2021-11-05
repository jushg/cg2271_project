#include "MKL25Z4.h"
#include "cmsis_os2.h"       
#include "init.h"
#include <stdio.h>
#include <inttypes.h>

osEventFlagsId_t ultrasonicFlag;
osMessageQueueId_t ultrasonicMsg;
//osMessageQueueId_t selfDriveMsg;
osSemaphoreId_t autoMoveSem;


/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void ultrasonic_tx_thread(void *argument) {
	for (;;) {
		//osEventFlagsWait(ultrasonicFlag, 0x00001, osFlagsNoClear, osWaitForever);
		PTB->PDOR |= MASK(PTB3_Pin);
		delay(0x11); // 10us pulse to begin ranging
		PTB->PDOR &= ~MASK(PTB3_Pin);
		delay(0x18e70); // 60ms
		osDelay(1);
	}
}

void ultrasonic_rx_thread(void *argument) {
	uint32_t timer=0;
	uint32_t centimeter=0;
	for (;;) {
		//osEventFlagsWait(ultrasonicFlag, 0x00001, osFlagsNoClear, osWaitForever);
		osMessageQueueGet(ultrasonicMsg, &timer, NULL, osWaitForever);
		centimeter = timer / 87; 				// * 10^6 / 58 * 32 / (48 * 10^6)
		if (centimeter < 30) {
			osSemaphoreRelease(autoMoveSem);	
		}		
		osDelay(1);
	}
}

void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
 	volatile uint32_t timer = 0;
	
	if (PTB->PDIR & MASK(PTB1_Pin)) {							// Rising Edge on Echo Pin = Wave begins to fire 
		TPM2->CNT = 0X0000;													// SET counter to 0			
		TPM2_C0SC |= TPM_CnSC_CHF_MASK;             // clear event
	} else {																			// Falling Edge = Wave returns 
		timer = TPM2_C0V;														
		TPM2_C0SC |= TPM_CnSC_CHF_MASK;
		osMessageQueuePut(ultrasonicMsg, &timer, NULL, 0);     //put current counter value into message queue
	}		
	delay(0x11);
}
