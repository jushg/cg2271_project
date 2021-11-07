#ifndef MOTOR_H
#define MOTOR_H
#include "MKL25Z4.h"                    // Device header
#include <math.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "init.h"

#define MAX_DUTY_CYCLE 7500 //(50Hz)
#define STRAIGHT_SPEED 0.65
osEventFlagsId_t motorFlag;
osMessageQueueId_t motorMsg;

typedef enum state {
	FORWARD,
	REVERSE,
	RIGHT,
	LEFT,
	STOP,
	START,
	FINISH
} state_t;

state_t state = START;

void stopMotors(){	
	LFW = 0;
	LBK = 0;
	RFW = 0;
	RBK = 0;
	state = STOP;
}

/** Move Reverse **/ 
void reverse() {
	LBK = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	RBK = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	state = REVERSE;
}

/** Move Forward **/
void forward() {
	LFW = MAX_DUTY_CYCLE * 0.2;
	RFW = MAX_DUTY_CYCLE * 0.2;
	state = FORWARD;
}

/** Rotate Right **/
void right() {
	LBK = MAX_DUTY_CYCLE/2;
	RFW = MAX_DUTY_CYCLE/2;
	state = RIGHT;
}

/** Rotate Left **/
void left() {
	LFW = MAX_DUTY_CYCLE/2;
	RBK = MAX_DUTY_CYCLE/2;
	state = LEFT;
}

/** Curved Forward Left**/
void leftforward() {
	LFW = MAX_DUTY_CYCLE;
	RFW = MAX_DUTY_CYCLE/6;
	state = FORWARD;
}

/** Curved Reverse Left**/
void leftreverse() {
	LBK = MAX_DUTY_CYCLE;
	RBK = MAX_DUTY_CYCLE/6;
	state = REVERSE;
}

/** Curved Forward Right**/
void rightforward() {
	LFW = MAX_DUTY_CYCLE/6;
	RFW = MAX_DUTY_CYCLE;
	state = FORWARD;
}

/** Curved Reverse Right**/
void rightreverse() {		
	LBK = MAX_DUTY_CYCLE/6;
	RBK = MAX_DUTY_CYCLE;
	state = REVERSE;
}

#endif
