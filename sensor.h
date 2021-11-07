#include "MKL25Z4.h"
#include "cmsis_os2.h"       
#include "init.h"
osEventFlagsId_t ultrasonicFlag;
osMessageQueueId_t ultrasonicMsg;
//osMessageQueueId_t selfDriveMsg;
osSemaphoreId_t autoMoveSem;
osSemaphoreId_t ultrasonicSemaphore;


/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

//Wenfeng
void ultrasonic_tx_thread(void *argument) {
	for (;;) {
		//Run the trigger
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
		//osSemaphoreRelease(autoMoveSem);	
		osMessageQueueGet(ultrasonicMsg, &timer, NULL, osWaitForever);
		centimeter = timer / 87; 				// * 10^6 / 58 * 32 / (48 * 10^6)
		if (centimeter < 30) {
			
			//osSemaphoreRelease(autoMoveSem);	
		}		
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

//Tan
/*

volatile int ultrasonicRising = 1;
volatile uint32_t ultrasonicReading = 1000;
void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	TPM2_STATUS |= TPM_STATUS_CH1F_MASK;
	if (ultrasonicRising) { //start of echo pin pulse
		TPM2_CNT = 0; 
		ultrasonicRising = 0;
		//Configure Input Compare Mode on Channel 1 to respond to falling edge
		TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
		TPM2_C1SC |= TPM_CnSC_ELSB_MASK;
		
	} else { //end of echo pin pulse
		ultrasonicReading = TPM2_C1V * 0.05715;
		ultrasonicRising = 1;
		NVIC_DisableIRQ(TPM2_IRQn);
	}
}
	
void tUltrasonic(void* Argument) {
	uint32_t reading =0;
	for (;;) {
		//osSemaphoreAcquire(ultrasonicSemaphore, osWaitForever);
		//Stop timer
		TPM2_SC &= ~TPM_SC_CMOD_MASK;
		
		//Enable Output Compare Mode on Channel 0, to generate 10 microsec high pulse when timer starts
		TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); 
		TPM2_C0SC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK);
		
		//Configure Input Compare Mode on Channel 1 to respond to rising edge
		TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
		TPM2_C1SC |= TPM_CnSC_ELSA_MASK;
		
		//Reset timer count value
		TPM2_CNT = 0;
		
		ultrasonicRising = 1;
		ultrasonicReading = 1000;
		NVIC_EnableIRQ(TPM2_IRQn);
		NVIC_ClearPendingIRQ(TPM2_IRQn);
		TPM2_SC |= TPM_SC_CMOD(1);
		
		osDelay(30);
		//LongMessageObject_t messageObject;
		//messageObject.message = ultrasonicReading;
		osMessageQueuePut(ultrasonicMsg, &ultrasonicReading, 0,0);
	}
}
*/


