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

/* Moving forward thread */
void tMotor_Forward(void *argument) {

}

/* Moving reverse thread */
void tMotor_Reverse(void *argument) {

}

/* Turning left thread */
void tMotor_Left(void *argument) {

}

/* Turning right thread */
void tMotor_Right(void *argument) {

}

/* Stop movement thread */
void tMotor_Stop(void *argument) {

}

/* Thread for decoding serial data and performing necessary actions */
void tBrain(void *argument) {

}

/* Thread for playing sound when bluetooth connection is established */
void tSound_opening(void *argument) {
  
}

/* Thread for playing sound while the robot is in the challenge run */
void tSound_running(void *argument) {
   
}

/* Thread for playing sound when the robot finishes the challenge run */
void tSound_ending(void *argument) {
   
}

void app_main (void *argument) {
 
  // ...
  for (;;) {}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
  initUART2(BAUD_RATE);
	initPWM();
	//forward();
	reverse();
 
  //osKernelInitialize();                 // Initialize CMSIS-RTOS
  //osThreadNew(app_main, NULL, NULL);    // Create application main thread
  //osKernelStart();                      // Start thread execution
  //for (;;) {}
	while(1) {
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
		else { stopMotors();}
	}
}
