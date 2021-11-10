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
#include "sensor.h"

osEventFlagsId_t ledFlag;
osEventFlagsId_t sensorFlag;

volatile state_t state = START;

 /*----------------------------------------------------------------------------
 * Application main threads
 *---------------------------------------------------------------------------*/

/* Red LEDs thread */
void tLed_red(void *argument) {    
	while(1) {
		osEventFlagsWait(ledFlag, 0x00001, osFlagsNoClear, osWaitForever);
		if(state == STOP || state == START || state == FINISH) {
			flashRedLEDs(250);
		}
		else {
			flashRedLEDs(500);
		}

	}		
}

/* Green LEDs thread */
void tLed_green(void *argument) {
	while(1) {
		osEventFlagsWait(ledFlag, 0x00001, osFlagsNoClear, osWaitForever);
		if(state == STOP || state == START || state == FINISH) {
			allGreenLightUp();
		}
		else {
			for(int i = 0; i < 8; ++i) {
				toggleGreenLED(i);
				osDelay(500);
			}
		}
	}
}

/* Motor thread */
void tMotor(void *argument) {
	uint8_t data;
	while(1) {
		osEventFlagsWait(motorFlag, 0x00001, osFlagsNoClear, osWaitForever);
		data = rxData;
		//osMessageQueueGet(motorMsg, &data, NULL, osWaitForever);
		if(data == UP_BUTTON_PRESSED ) {
			forward();
			state = FORWARD;
		}
		else if(data == DOWN_BUTTON_PRESSED) {
			reverse();
			state = REVERSE;
		}
		else if(data == LEFT_BUTTON_PRESSED) {
			left();
			state = LEFT;
		}
		else if(data == RIGHT_BUTTON_PRESSED) {
			right();
			state = RIGHT;
		}
		else if(data == RIGHT_FORWARD_BUTTON_PRESSED) {
			rightforward();
			state = FORWARD;
		}
		else if(data == RIGHT_REVERSE_BUTTON_PRESSED) {
			rightreverse();
			state = REVERSE;
		}
		else if(data == LEFT_FORWARD_BUTTON_PRESSED) {
			leftforward();
			state = FORWARD;
		}
		else if(data == LEFT_REVERSE_BUTTON_PRESSED) {
			leftreverse();
			state = REVERSE;
		}
		else if (data == ALL_BUTTON_RELEASED) { 
			stopMotors();
			state = STOP;
		}
		else if (data == THE_END) {
			stopMotors();
			state = FINISH;
		}
	}
}


/* Thread for decoding serial data and performing necessary actions */
void tBrain(void *argument) {
	//uint8_t rxData;
	while(1) {
		//osMessageQueueGet(uartMsg, &rxData, NULL, osWaitForever);
		left();
		osDelay(500);
		right();
		osDelay(500);
		if (rxData == CONNECT) {	
			//when the ESP restarts
			//Clear all the current flag
			osEventFlagsClear(ledFlag, 0x00001);
			osEventFlagsClear(audioFlag, 0x00007);	
			osEventFlagsClear(motorFlag, 0x00003);
			
			osEventFlagsSet(audioFlag, 0x00001);	 //Play connected tunes	
			//Flash 2 times
			flashGreenLEDs(250);
			flashGreenLEDs(250);
			osEventFlagsSet(ledFlag, 0x00001);
			osEventFlagsSet(audioFlag, 0x00002);
			rxData = UNIDENTIFIED;
		}
		else if (rxData <= 9) { // manually control the motors
			osEventFlagsClear(motorFlag, 0x00003); 			
			osEventFlagsSet(motorFlag, 0x00001);
			//osMessageQueuePut(motorMsg, &rxData, NULL, 0);
		}
		else if (rxData == AUTO) { //auto drive
			osEventFlagsClear(motorFlag, 0x00003); //Clear the last motor option
			osEventFlagsClear(audioFlag, 0x00007);	//Clear the last audio option
			
			osEventFlagsSet(motorFlag, 0x00002);
			osEventFlagsSet(audioFlag, 0x00002);
			osEventFlagsSet(ultrasonicFlag, 0x00001);
			
			rxData = UNIDENTIFIED;

			//osMessageQueuePut(motorMsg, &rxData, NULL, 0);
		}
		else if (rxData == THE_END) {
			stopMotors();
			//osMessageQueuePut(motorMsg, &rxData, NULL, 0);
			//Play the end song
			osEventFlagsClear(audioFlag, 0x00007);	
			osEventFlagsSet(audioFlag, 0x00004);
		}
	}

}

/* Thread for playing sound when wifi connection is established */
void tSound_opening(void *argument) {
	int melody_len = sizeof(connected_melody)/sizeof(int);
	while(1) {
		osEventFlagsWait(audioFlag, 0x00001, osFlagsNoClear, osWaitForever);
		sing(connected_melody, connected_tempo, melody_len, 0x00001);
		osEventFlagsClear(audioFlag, 0x00007);	
	}
}

/* Thread for playing sound while the robot is in the challenge run */
void tSound_running(void *argument) {
	int melody_len = sizeof(underworld_melody)/sizeof(int);
   while(1) {
		osEventFlagsWait(audioFlag, 0x00002, osFlagsNoClear, osWaitForever);
		sing(underworld_melody, underworld_tempo, melody_len, 0x00002);
	}
}

/* Thread for playing sound when the robot finishes the challenge run */
void tSound_ending(void *argument) {
	//int melody_len = sizeof(gurenge)/sizeof(int);
	int melody_len = sizeof(birthday)/sizeof(int);

  while(1) {
		osEventFlagsWait(audioFlag, 0x00004, osFlagsNoClear, osWaitForever);
		sing(birthday, birthday_tempo, melody_len, 0x00004);
		osEventFlagsClear(audioFlag, 0x00007);	
	}
}

void tAuto_driving(void *argument) {
  while(1) {
		osEventFlagsWait(motorFlag, 0x00002, osFlagsNoClear, osWaitForever);
		forward();
		osSemaphoreAcquire(autoMoveSem, osWaitForever);
		osEventFlagsClear(ultrasonicFlag, 0x00001);
		stopMotors();
		//left();
		//osDelay(500);
		//forward();
		//osDelay(500);
		//left();
		//osDelay(500);
		
		osEventFlagsClear(motorFlag, 0x00003);	
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
  initUART2(BAUD_RATE);
	//initPWM();
	initMotor();
	initAudio();
	initLED();
	initUltrasonic();
	
	offGreenLEDs();
	offRedLEDs();
	stopMotors();

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	motorFlag = osEventFlagsNew(NULL);
	audioFlag = osEventFlagsNew(NULL);
	ledFlag = osEventFlagsNew(NULL);
	ultrasonicFlag = osEventFlagsNew(NULL);
	
	autoMoveSem = osSemaphoreNew(1, 0, NULL); // Semaphore for auto movement
	

	osThreadNew(tBrain,NULL,NULL);
	
	osThreadNew(tMotor,NULL,NULL);
	osThreadNew(tAuto_driving,NULL,NULL);

	osThreadNew(tSound_ending,NULL,NULL);
	osThreadNew(tSound_opening,NULL,NULL);
	osThreadNew(tSound_running,NULL,NULL);
	
	osThreadNew(tLed_green,NULL,NULL);
	osThreadNew(tLed_red,NULL,NULL);
	
	osThreadNew(ultrasonic_tx_thread, NULL, NULL);
	osThreadNew(ultrasonic_rx_thread, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
