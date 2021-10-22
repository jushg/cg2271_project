/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"                    // Device header
#include "init.h"
#include "motor.h"
#include "uart.h"
#include "audio.h"
#include "led.h"


volatile state_t state = FORWARD;

// Received data from BT06 module
volatile uint32_t rx_data = 0;

/* Interupt for capturing serial data */
void UART2_IRQHandler(void) {
    NVIC_ClearPendingIRQ(UART2_IRQn);
    if (UART2->S1 & UART_S1_RDRF_MASK) {
        rx_data = UART2_D;
    }   
    //Clear INT Flag
    PORTE->ISFR |= MASK(UART_RX_PORTE23);
}


 /*----------------------------------------------------------------------------
 * Application main threads
 *---------------------------------------------------------------------------*/

/* Red LEDs thread */
void tLed_red(void *argument) {    

}

/* Green LEDs thread */
void tLed_green(void *argument) {

}

/* Motor thread */
void tMotor(void *argument) {
	for(;;) {
		if(rx_data == UP_BUTTON_PRESSED ) {
			forward();
		}
		else if(rx_data == DOWN_BUTTON_PRESSED) {
			reverse();
		}
		else if(rx_data == LEFT_BUTTON_PRESSED) {
			left();
		}
		else if(rx_data == RIGHT_BUTTON_PRESSED) {
			right();
		}
		else if (rx_data == ALL_BUTTON_RELEASED) { stopMotors();}
	}
}


/* Thread for decoding serial data and performing necessary actions */
void tBrain(void *argument) {

}

/* Thread for playing sound when bluetooth connection is established */
void tSound_opening(void *argument) {
	for(;;) {
		
	}
}

/* Thread for playing sound while the robot is in the challenge run */
void tSound_running(void *argument) {
   for(;;) {
		int melody_len = sizeof(underworld_melody)/sizeof(int);
		sing(underworld_melody, underworld_tempo, melody_len, 0x00004);
	}
}

/* Thread for playing sound when the robot finishes the challenge run */
void tSound_ending(void *argument) {
   for(;;) {
		
	}
}


 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
  initUART2(BAUD_RATE);
	initPWM();
	initAudio();
	initLED();
	
	//forward();
	//reverse();
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	osThreadNew(tMotor,NULL,NULL);
	//osThreadNew(tBrain,NULL,NULL);
	//osThreadNew(tSound_ending,NULL,NULL);
	//osThreadNew(tSound_opening,NULL,NULL);
	//osThreadNew(tSound_running,NULL,NULL);
	//osThreadNew(tLed_green,NULL,NULL);
	//osThreadNew(tLed_red,NULL,NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}

	
}
