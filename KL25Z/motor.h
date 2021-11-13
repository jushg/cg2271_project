#ifndef MOTOR_H
#define MOTOR_H
#include "init.h"
#include "util.h"

#define DUTY_CYCLE 7500 //(50Hz)

//Using multipliers to balance the power of the two sides
#define LEFT_POWER DUTY_CYCLE * 0.845
#define RIGHT_POWER DUTY_CYCLE * 1

//Speed when moving forward
#define STRAIGHT_SPEED 0.7

void stopMotors(void);
void reverse(void) ;
void forward(void) ;
void left(void) ;
void leftforward(void) ;
void leftreverse(void) ;
void right(void) ;
void rightforward(void) ;
void rightreverse(void) ;

#endif
