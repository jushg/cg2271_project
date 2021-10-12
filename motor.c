#include "MKL25Z4.h"                    // Device header
#include "motor.h"


void stopMotors(){
	//Stop Left Side
	TPM1_C0V = 0; 
	TPM1_C1V = 0; 
	//Stop Right Side
	TPM2_C0V = 0; 
	TPM2_C1V = 0; 
}

/** Move Reverse **/ 
void reverse() {
	TPM1_C0V = 0;
	TPM2_C0V = 0;
	TPM1_C1V = MAX_DUTY_CYCLE;
	TPM2_C1V = MAX_DUTY_CYCLE;
}

/** Move Forward **/
void forward() {
	TPM1_C1V = 0;
	TPM2_C1V = 0;
	TPM1_C0V = MAX_DUTY_CYCLE;
	TPM2_C0V = MAX_DUTY_CYCLE;
}

/** Rotate Right **/
void right() {
	TPM1_C0V = 0;
	TPM2_C1V = 0;
	TPM1_C1V = MAX_DUTY_CYCLE/3;
	TPM2_C0V = MAX_DUTY_CYCLE/3;
}

/** Rotate Left **/
void left() {
	TPM1_C0V = 0;
	TPM2_C1V = 0;
	TPM2_C0V = MAX_DUTY_CYCLE/3;
	TPM1_C1V = MAX_DUTY_CYCLE/3;
}

/** Curved Forward Left**/
void leftforward() {
	TPM1_C1V = 0;
	TPM2_C1V = 0;
	TPM2_C0V = MAX_DUTY_CYCLE;
	TPM1_C0V = MAX_DUTY_CYCLE/6;
}

/** Curved Reverse Left**/
void leftreverse() {
	TPM1_C0V = 0;
	TPM2_C0V = 0;
	TPM2_C1V = MAX_DUTY_CYCLE;
	TPM1_C1V = MAX_DUTY_CYCLE/6;	
}

/** Curved Forward Right**/
void rightforward() {
	TPM1_C1V = 0;
	TPM2_C1V = 0;
	TPM2_C0V = MAX_DUTY_CYCLE/6;
	TPM1_C0V = MAX_DUTY_CYCLE;
}

/** Curved Reverse Right**/
void rightreverse() {
	TPM1_C0V = 0;
	TPM2_C0V = 0;
	TPM2_C1V = MAX_DUTY_CYCLE/6;
	TPM1_C1V = MAX_DUTY_CYCLE;
}






