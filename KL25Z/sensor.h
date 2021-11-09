#include "MKL25Z4.h"
#include "cmsis_os2.h"       
#include "init.h"
osEventFlagsId_t ultrasonicFlag;
osMessageQueueId_t ultrasonicMsg;


/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void tUltrasonic_trigger(void *argument) {
	for (;;) {
		//Run the trigger
		osEventFlagsWait(ultrasonicFlag, 0x00001, osFlagsNoClear, osWaitForever);
		PTB->PDOR |= MASK(PTB3_Pin);
		delay(0x11); // 10us pulse to begin ranging
		PTB->PDOR &= ~MASK(PTB3_Pin);
		delay(0x18e70); // 60ms
		osDelay(1);
	}
}

volatile uint32_t timer=20;
void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	
	if (PTB->PDIR & MASK(PTB1_Pin)) {							// Rising Edge
		TPM2->CNT = 0X0000;
		TPM2_C0SC |= TPM_CnSC_CHF_MASK;
	} else {																			// Falling Edge
		timer = TPM2_C0V;	
		TPM2_C0SC |= TPM_CnSC_CHF_MASK;
		osMessageQueuePut(ultrasonicMsg, &timer, NULL, 0);
	}		
	delay(0x11);
}

