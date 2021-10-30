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

void stopMotors(){
	//Stop Left Side
	//TPM1_C0V = 0; 
	//TPM1_C1V = 0; 
	//Stop Right Side
	//TPM2_C0V = 0; 
	//TPM2_C1V = 0; 
		
	LFW = 0;
	LBK = 0;
	RFW = 0;
	RBK = 0;
}

/** Move Reverse **/ 
void reverse() {
	//TPM1_C0V = 0;
	//TPM2_C0V = 0;
	//TPM1_C1V = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	//TPM2_C1V = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
		
	LFW = 0;
	LBK = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	RFW = 0;
	RBK = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
}

/** Move Forward **/
void forward() {
	//TPM1_C1V = 0;
	//TPM2_C1V = 0;
	//TPM1_C0V = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	//TPM2_C0V = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
		
	LFW = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	LBK = 0;
	RFW = MAX_DUTY_CYCLE * STRAIGHT_SPEED;
	RBK = 0;
}

/** Rotate Right **/
void right() {
	//TPM1_C1V = 0;
	//TPM2_C0V = 0;
	//TPM1_C0V = MAX_DUTY_CYCLE/3;
	//TPM2_C1V = MAX_DUTY_CYCLE/3;
		
	LFW = 0;
	LBK = MAX_DUTY_CYCLE/3;
	RFW = MAX_DUTY_CYCLE/3;
	RBK = 0;
}

/** Rotate Left **/
void left() {
	//TPM1_C0V = 0;
	//TPM2_C1V = 0;
	//TPM2_C0V = MAX_DUTY_CYCLE/3;
	//TPM1_C1V = MAX_DUTY_CYCLE/3;
		
	LFW = MAX_DUTY_CYCLE/3;
	LBK = 0;
	RFW = 0;
	RBK = MAX_DUTY_CYCLE/3;
}

/** Curved Forward Left**/
void leftforward() {
	//TPM1_C1V = 0;
	//TPM2_C1V = 0;
	//TPM2_C0V = MAX_DUTY_CYCLE;
	//TPM1_C0V = MAX_DUTY_CYCLE/6;
	
		
	LFW = MAX_DUTY_CYCLE/6;
	LBK = 0;
	RFW = MAX_DUTY_CYCLE;
	RBK = 0;
}

/** Curved Reverse Left**/
void leftreverse() {
	//TPM1_C0V = 0;
	//TPM2_C0V = 0;
	//TPM2_C1V = MAX_DUTY_CYCLE;
	//TPM1_C1V = MAX_DUTY_CYCLE/6;	
	
		
	LFW = 0;
	LBK = MAX_DUTY_CYCLE/6;
	RFW = 0;
	RBK = MAX_DUTY_CYCLE;
}

/** Curved Forward Right**/
void rightforward() {
	//TPM1_C1V = 0;
	//TPM2_C1V = 0;
	//TPM2_C0V = MAX_DUTY_CYCLE/6;
	//TPM1_C0V = MAX_DUTY_CYCLE;
	
		
	LFW = MAX_DUTY_CYCLE;
	LBK = 0;
	RFW = MAX_DUTY_CYCLE/6;
	RBK = 0;
}

/** Curved Reverse Right**/
void rightreverse() {
	//TPM1_C0V = 0;
	//TPM2_C0V = 0;
	//TPM2_C1V = MAX_DUTY_CYCLE/6;
	//TPM1_C1V = MAX_DUTY_CYCLE;
	
		
	LFW = 0;
	LBK = MAX_DUTY_CYCLE;
	RFW = 0;
	RBK = MAX_DUTY_CYCLE/6;
}

#endif
