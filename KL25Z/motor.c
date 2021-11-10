#include "motor.h"

void stopMotors(){	
	LFW = 0;
	LBK = 0;
	RFW = 0;
	RBK = 0;
	state = STOP;
}

void reverse() {
	LBK = LEFT_POWER * STRAIGHT_SPEED;
	RBK = RIGHT_POWER * STRAIGHT_SPEED;
	state = REVERSE;
}

void forward() {
	LFW = LEFT_POWER * STRAIGHT_SPEED;
	RFW = RIGHT_POWER * STRAIGHT_SPEED;
	state = FORWARD;
}

void right() {
	LBK = LEFT_POWER * (2/5);
	RFW = RIGHT_POWER* (2/5);
	state = RIGHT;
}

void left() {
	LFW = LEFT_POWER * (2/5);
	RBK = RIGHT_POWER* (2/5);
	state = LEFT;
}

void leftforward() {
	LFW = LEFT_POWER;
	RFW = RIGHT_POWER/6;
	state = FORWARD;
}

void leftreverse() {
	LBK = LEFT_POWER;
	RBK = RIGHT_POWER/6;
	state = REVERSE;
}

void rightforward() {
	LFW = LEFT_POWER/6;
	RFW = RIGHT_POWER;
	state = FORWARD;
}

void rightreverse() {		
	LBK = LEFT_POWER/6;
	RBK = RIGHT_POWER;
	state = REVERSE;
}