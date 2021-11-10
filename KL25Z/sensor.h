#ifndef SENSOR_H
#define SENSOR_H
  
#include "init.h"
#include "util.h"

void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	uint32_t timer=0;
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
#endif

