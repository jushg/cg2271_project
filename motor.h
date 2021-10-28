#ifndef MOTOR_H
#define MOTOR_H
#include "MKL25Z4.h"                    // Device header
#include <math.h>

#define MAX_DUTY_CYCLE 7500 //(50Hz)

#define STRAIGHT_SPEED 0.65

typedef enum state {
	FORWARD,
	REVERSE,
	RIGHT,
	LEFT,
	STOP,
	START,
	FINISH
} state_t;

void stopMotors(void);
void reverse(void);
void forward(void);
void right(void);
void left(void);
void leftforward(void);
void leftreverse(void);
void rightforward(void);
void rightreverse(void);

#endif
