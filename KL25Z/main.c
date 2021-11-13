/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "init.h"
#include "motor.h"
#include "uart.h"
#include "audio.h"
#include "led.h"
#include "sensor.h"
#include "util.h"

const osThreadAttr_t brainPriority = {
		.priority = osPriorityHigh4
};
const osThreadAttr_t motorPriority = {
		.priority = osPriorityAboveNormal
};

osEventFlagsId_t ledFlag;
osEventFlagsId_t sensorFlag;
osEventFlagsId_t motorFlag;
osEventFlagsId_t audioFlag;
osEventFlagsId_t ultrasonicFlag;

osMessageQueueId_t motorMsg;
osMessageQueueId_t uartMsg;
osMessageQueueId_t ultrasonicMsg;

state_t state ;

 /*----------------------------------------------------------------------------
 * Application main threads
 *---------------------------------------------------------------------------*/

/* Red LEDs thread */
void tLed_red(void *argument) {    
	while(1) {
		osEventFlagsWait(ledFlag, 0x00001, osFlagsNoClear, osWaitForever);
		if(state == STOP ) {
			flashRedLEDs(250);
		}
		else {
			flashRedLEDs(500);
		}
	}		
}

/* Green LEDs thread */
void tLed_green(void *argument) {
	int toggleNumber = 0;
	while(1) {
		osEventFlagsWait(ledFlag, 0x00001, osFlagsNoClear, osWaitForever);
		if(state == STOP || state == START || state == FINISH) {
			allGreenLightUp();
		}
		else {
			toggleNumber = toggleNumber >= 8 ? 0: toggleNumber;
			toggleGreenLED(toggleNumber);
			osDelay(250);
			toggleNumber++;
		}
	}
}

/* Motor thread */
void tMotor(void *argument) {
	uint8_t data;
	while(1) {
		osEventFlagsWait(motorFlag, 0x00001, osFlagsNoClear, osWaitForever);
		osMessageQueueGet(motorMsg, &data, NULL, osWaitForever);
		if(data == UP_BUTTON) {
			forward();
		}
		else if(data == DOWN_BUTTON) {
			reverse();
		}
		else if(data == LEFT_BUTTON) {
			left();
			osDelay(150);
			stopMotors();
			
		}
		else if(data == RIGHT_BUTTON) {
			right();
			osDelay(150);
			stopMotors();
		}
		else if(data == RIGHT_FORWARD_BUTTON) {
			rightforward();
		}
		else if(data == RIGHT_REVERSE_BUTTON) {
			rightreverse();
		}
		else if(data == LEFT_FORWARD_BUTTON) {
			leftforward();
		}
		else if(data == LEFT_REVERSE_BUTTON) {
			leftreverse();
		}
		else if (data == ALL_BUTTON_RELEASED) { 
			stopMotors();
		}
	}
}


/* Thread for decoding serial data and performing necessary actions */
void tBrain(void *argument) {
	uint8_t rxData;
	while(1) {
		osMessageQueueGet(uartMsg, &rxData, NULL, osWaitForever);
		if (rxData == CONNECTED_SIGNAL) {	//when the ESP restarts/initially connected
			//Clear all the current flag
			osEventFlagsClear(ledFlag, 0x00001);
			osEventFlagsClear(audioFlag, 0x00007);	
			osEventFlagsClear(motorFlag, 0x00003);

			//Play connected tunes	
			osEventFlagsSet(audioFlag, 0x00001);	 
			//Flash Green LEDs 2 times
			flashGreenLEDs(250);
			flashGreenLEDs(250);

			//Set flag for running audio and LED threads 
			osEventFlagsSet(ledFlag, 0x00001);
			osEventFlagsSet(audioFlag, 0x00002);
		}
		else if (rxData <= 9) { // manually control the motors
			//Clear the previous motor flag
			osEventFlagsClear(motorFlag, 0x00003); 			
			
			//Set manual mode
			osEventFlagsSet(motorFlag, 0x00001);
			osMessageQueuePut(motorMsg, &rxData, NULL, 0);
		}
		else if (rxData == AUTO_BUTTON) { //auto drive mode
			//Clear the previous motor and audio flags
			osEventFlagsClear(motorFlag, 0x00003); 
			osEventFlagsClear(audioFlag, 0x00007);	
			
			//Set auto mode
			osEventFlagsSet(motorFlag, 0x00002);
			osEventFlagsSet(audioFlag, 0x00002);
		}
		else if (rxData == END_BUTTON) { // the end tune play
			stopMotors();
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
	int melody_len = sizeof(twinkle_stars)/sizeof(int);
   while(1) {
		osEventFlagsWait(audioFlag, 0x00002, osFlagsNoClear, osWaitForever);
		sing(twinkle_stars, twinkle_stars_tempo, melody_len, 0x00002);
	}
}

/* Thread for playing sound when the robot finishes the challenge run */
void tSound_ending(void *argument) {
	int melody_len = sizeof(birthday)/sizeof(int);
  while(1) {
		osEventFlagsWait(audioFlag, 0x00004, osFlagsNoClear, osWaitForever);
		sing(birthday, birthday_tempo, melody_len, 0x00004);
		osEventFlagsClear(audioFlag, 0x00007);	
	}
}

/* Thread for the trigger pin of the ultrasonic sensor */
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

#define STOP_DISTANCE 50
#define TURN_DELAY 200
#define FORWARD_DELAY 300
#define STOP_DELAY 100

/* Thread for the automatic mode of the robot */
void tAuto_driving(void *argument) {
	uint32_t timer= 0;
	uint32_t centimeter;
  while(1) {
		osEventFlagsWait(motorFlag, 0x00002, osFlagsNoClear, osWaitForever);
		
		forward();
		
		//Move until find the obstacle
		osEventFlagsSet(ultrasonicFlag, 0x00001);
		osMessageQueueReset(ultrasonicMsg);
		centimeter = 200;
		while (centimeter > STOP_DISTANCE) {
			//get ultrasonic reading
			osStatus_t status = osMessageQueueGet(ultrasonicMsg, &timer, NULL, osWaitForever);
			if (status == osOK) {
				centimeter = timer / 87; 				// * 10^6 / 58 * 32 / (48 * 10^6)
			}
		}
		osEventFlagsClear(ultrasonicFlag, 0x00001);
		
		stopMotors();
		osDelay(STOP_DELAY);
		
		//Move into the loop
		left();
		osDelay(TURN_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2.1);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2.25);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2.05);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY*0.95);
		stopMotors();
		osDelay(STOP_DELAY);
		
		left();
		osDelay(TURN_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		//Finish the loop, return back
		forward();
		
		//Move until detect the stop obstacle
		osEventFlagsSet(ultrasonicFlag, 0x00001);
		osMessageQueueReset(ultrasonicMsg);
		centimeter = 200;
		while (centimeter > STOP_DISTANCE) {
			//get ultrasonic reading
			osStatus_t status = osMessageQueueGet(ultrasonicMsg, &timer, NULL, osWaitForever);
			if (status == osOK) {
				centimeter = timer / 87; 				// * 10^6 / 58 * 32 / (48 * 10^6)
			}
		}
		osEventFlagsClear(ultrasonicFlag, 0x00001);
		stopMotors();
		osDelay(STOP_DELAY);
		osEventFlagsClear(motorFlag, 0x00003);	
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	
	//Ports Initialization
	initClockGate();
  initUART2(BAUD_RATE);
	initMotor();
	initAudio();
	initLED();
	initUltrasonic(); 
	
	//Initial setup to stabilize the robot
	offGreenLEDs();
	offRedLEDs();
	stopMotors();

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	//Event flags
	motorFlag = osEventFlagsNew(NULL);
	audioFlag = osEventFlagsNew(NULL);
	ledFlag = osEventFlagsNew(NULL);
	ultrasonicFlag = osEventFlagsNew(NULL);
	
	//Queues
	ultrasonicMsg = osMessageQueueNew(4, sizeof(uint32_t), NULL);
	motorMsg = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	uartMsg = osMessageQueueNew(10, sizeof(uint8_t), NULL);

	//Main thread
	osThreadNew(tBrain,NULL, &brainPriority);
	
	//Driving threads
	osThreadNew(tMotor,NULL,&motorPriority);
	osThreadNew(tAuto_driving,NULL,&motorPriority);

	//Sound threads
  osThreadNew(tSound_ending,NULL,NULL);
	osThreadNew(tSound_opening,NULL,NULL);
	osThreadNew(tSound_running,NULL,NULL);
	
	//LED threads
	osThreadNew(tLed_green,NULL,NULL);
	osThreadNew(tLed_red,NULL,NULL);
	
	//Ultrasonic thread
	osThreadNew(tUltrasonic_trigger, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
