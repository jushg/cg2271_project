<<<<<<< HEAD
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
	int toggleNumber = 0;
	while(1) {
		osEventFlagsWait(ledFlag, 0x00001, osFlagsNoClear, osWaitForever);
		if(state == STOP || state == START || state == FINISH) {
			allGreenLightUp();
		}
		else {
			for(int i = 0; i < 8; ++i) {
				toggleGreenLED(i);
				osDelay(250);
			}
		}
	}
}

/* Motor thread */
void tMotor(void *argument) {
	uint8_t data;
	while(1) {
		osEventFlagsWait(motorFlag, 0x00001, osFlagsNoClear, osWaitForever);
		osMessageQueueGet(motorMsg, &data, NULL, osWaitForever);
		if(data == UP_BUTTON_PRESSED ) {
			forward();
			//state = FORWARD;
		}
		else if(data == DOWN_BUTTON_PRESSED) {
			reverse();
			//state = REVERSE;
		}
		else if(data == LEFT_BUTTON_PRESSED) {
			left();
			//state = LEFT;
		}
		else if(data == RIGHT_BUTTON_PRESSED) {
			right();
			//state = RIGHT;
		}
		else if(data == RIGHT_FORWARD_BUTTON_PRESSED) {
			rightforward();
			//state = FORWARD;
		}
		else if(data == RIGHT_REVERSE_BUTTON_PRESSED) {
			rightreverse();
			//state = REVERSE;
		}
		else if(data == LEFT_FORWARD_BUTTON_PRESSED) {
			leftforward();
			//state = FORWARD;
		}
		else if(data == LEFT_REVERSE_BUTTON_PRESSED) {
			leftreverse();
			//state = REVERSE;
		}
		else if (data == ALL_BUTTON_RELEASED) { 
			stopMotors();
			//state = STOP;
		}
		else if (data == THE_END) {
			stopMotors();
			//state = FINISH;
		}
	}
}


/* Thread for decoding serial data and performing necessary actions */
void tBrain(void *argument) {
	uint8_t rxData;
	while(1) {
		osMessageQueueGet(uartMsg, &rxData, NULL, osWaitForever);
		
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
			osMessageQueuePut(motorMsg, &rxData, NULL, 0);
		}
		else if (rxData == AUTO) { //auto drive
			osEventFlagsClear(motorFlag, 0x00003); //Clear the last motor option
			osEventFlagsClear(audioFlag, 0x00007);	//Clear the last audio option
			
			osEventFlagsSet(motorFlag, 0x00002);
			osEventFlagsSet(audioFlag, 0x00002);
			
			rxData = UNIDENTIFIED;
			//osEventFlagsSet(ultrasonicFlag, 0x00001);

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


#define STOP_DISTANCE 30

void tAuto_driving(void *argument) {
	uint32_t TURN_DELAY = 280;
	uint32_t FORWARD_DELAY = 350;
	uint32_t STOP_DELAY = 500;
	uint32_t timer= 0;
	uint32_t centimeter;
  while(1) {
		osEventFlagsWait(motorFlag, 0x00002, osFlagsNoClear, osWaitForever);
		
		forward();
		
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
		
		
		left();
		osDelay(TURN_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		left();
		osDelay(TURN_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		//Finish the loop, return back
		
		forward();
		
		
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
	initClockGate();
  initUART2(BAUD_RATE);
	initMotor();
	initAudio();
	initLED();
	initUltrasonicPWM(); 
	
	offGreenLEDs();
	offRedLEDs();
	stopMotors();

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	motorFlag = osEventFlagsNew(NULL);
	audioFlag = osEventFlagsNew(NULL);
	ledFlag = osEventFlagsNew(NULL);
	ultrasonicFlag = osEventFlagsNew(NULL);
	
	ultrasonicMsg = osMessageQueueNew(4, sizeof(uint32_t), NULL);
	
	motorMsg = osMessageQueueNew(4, sizeof(uint8_t), NULL);
	uartMsg = osMessageQueueNew(32, sizeof(uint8_t), NULL);

	osThreadNew(tBrain,NULL,NULL);
	
	osThreadNew(tMotor,NULL,NULL);
	osThreadNew(tAuto_driving,NULL,NULL);

  osThreadNew(tSound_ending,NULL,NULL);
	osThreadNew(tSound_opening,NULL,NULL);
	osThreadNew(tSound_running,NULL,NULL);
	
	osThreadNew(tLed_green,NULL,NULL);
	osThreadNew(tLed_red,NULL,NULL);
	
	osThreadNew(tUltrasonic_trigger, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
	left();
	osDelay(280);
	right();
	osDelay(280);
	for (;;) {}
}
=======
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
		}
	}
}

/* Motor thread */
void tMotor(void *argument) {
	uint8_t data;
	while(1) {
		osEventFlagsWait(motorFlag, 0x00001, osFlagsNoClear, osWaitForever);
		osMessageQueueGet(motorMsg, &data, NULL, osWaitForever);
		if(data == UP_BUTTON_PRESSED ) {
			forward();
		}
		else if(data == DOWN_BUTTON_PRESSED) {
			reverse();
		}
		else if(data == LEFT_BUTTON_PRESSED) {
			left();
		}
		else if(data == RIGHT_BUTTON_PRESSED) {
			right();
		}
		else if(data == RIGHT_FORWARD_BUTTON_PRESSED) {
			rightforward();
		}
		else if(data == RIGHT_REVERSE_BUTTON_PRESSED) {
			rightreverse();
		}
		else if(data == LEFT_FORWARD_BUTTON_PRESSED) {
			leftforward();
		}
		else if(data == LEFT_REVERSE_BUTTON_PRESSED) {
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
			
		}
		else if (rxData <= 9) { // manually control the motors
			osEventFlagsClear(motorFlag, 0x00003); 			
			osEventFlagsSet(motorFlag, 0x00001);
			osMessageQueuePut(motorMsg, &rxData, NULL, 0);
		}
		else if (rxData == AUTO) { //auto drive
			osEventFlagsClear(motorFlag, 0x00003); //Clear the last motor option
			osEventFlagsClear(audioFlag, 0x00007);	//Clear the last audio option
			
			osEventFlagsSet(motorFlag, 0x00002);
			osEventFlagsSet(audioFlag, 0x00002);
		}
		else if (rxData == THE_END) {
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

#define STOP_DISTANCE 30
#define TURN_DELAY 280
#define FORWARD_DELAY 350
#define STOP_DELAY 500

void tAuto_driving(void *argument) {
	uint32_t timer= 0;
	uint32_t centimeter;
  while(1) {
		osEventFlagsWait(motorFlag, 0x00002, osFlagsNoClear, osWaitForever);
		
		forward();
		
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
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		right();
		osDelay(TURN_DELAY*2);
		stopMotors();
		osDelay(STOP_DELAY);
		
		forward();
		osDelay(FORWARD_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		
		left();
		osDelay(TURN_DELAY);
		stopMotors();
		osDelay(STOP_DELAY);
		//Finish the loop, return back
		
		forward();
		
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
	
	initClockGate();
  initUART2(BAUD_RATE);
	initMotor();
	initAudio();
	initLED();
	initUltrasonic(); 
	
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
	uartMsg = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	//Main thread
	osThreadNew(tBrain,NULL,NULL);
	
	//Driving threads
	osThreadNew(tMotor,NULL,NULL);
	osThreadNew(tAuto_driving,NULL,NULL);

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
>>>>>>> 94808f2ee9cdba45f5bca2d76bc3a1b2db963bd9
