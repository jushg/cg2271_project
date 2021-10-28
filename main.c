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


volatile state_t state = START;

// Received data from ESP32
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
	for(;;) {
		if(state == STOP || state == START || state == FINISH) {
			allRedLightUp();
			osDelay(250);
			offRedLEDs();
			osDelay(250);
		}
		else {
			allRedLightUp();
			osDelay(500);
			offRedLEDs();
			osDelay(500);
		}
	}
}

/* Green LEDs thread */
void tLed_green(void *argument) {
	for(;;) {
		if(state == STOP || state == START || state == FINISH) {
			allGreenLightUp();
		}
		else {
			for(int i = 0; i < 8; ++i) {
				toggleGreenLED(i);
				osDelay(100);
			}
		}
	}
}

/* Motor thread */
void tMotor(void *argument) {
	for(;;) {
		if(rx_data == UP_BUTTON_PRESSED ) {
			forward();
			state = FORWARD;
		}
		else if(rx_data == DOWN_BUTTON_PRESSED) {
			reverse();
			state = REVERSE;
		}
		else if(rx_data == LEFT_BUTTON_PRESSED) {
			left();
			state = LEFT;
		}
		else if(rx_data == RIGHT_BUTTON_PRESSED) {
			right();
			state = RIGHT;
		}
		else if(rx_data == RIGHT_FORWARD_BUTTON_PRESSED) {
			rightforward();
			state = FORWARD;
		}
		else if(rx_data == RIGHT_REVERSE_BUTTON_PRESSED) {
			rightreverse();
			state = REVERSE;
		}
		else if(rx_data == LEFT_FORWARD_BUTTON_PRESSED) {
			leftforward();
			state = FORWARD;
		}
		else if(rx_data == LEFT_REVERSE_BUTTON_PRESSED) {
			leftreverse();
			state = REVERSE;
		}
		else if (rx_data == ALL_BUTTON_RELEASED) { 
			stopMotors();
			state = STOP;
		}
		else if (rx_data == FINISH_BUTTON_PRESSED) 
		{
			stopMotors();
			state = FINISH;
		}
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
	int melody_len = sizeof(underworld_melody)/sizeof(int);
   for(;;) {
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
	
	offGreenLEDs();
	offRedLEDs();
	

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	osThreadNew(tMotor,NULL,NULL);
	//osThreadNew(tBrain,NULL,NULL);
	//osThreadNew(tSound_ending,NULL,NULL);
	//osThreadNew(tSound_opening,NULL,NULL);
	//osThreadNew(tSound_running,NULL,NULL);
	osThreadNew(tLed_green,NULL,NULL);
	osThreadNew(tLed_red,NULL,NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
