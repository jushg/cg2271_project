#ifndef UTIL_H
#define UTIL_H

#include "RTE_Components.h"
#include "MKL25Z4.h"                    // Device header
#include  CMSIS_device_header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include <math.h>

typedef enum state {
	FORWARD,
	REVERSE,
	RIGHT,
	LEFT,
	STOP,
	START,
	FINISH
} state_t;

extern state_t state ;
extern osEventFlagsId_t ledFlag;
extern osEventFlagsId_t sensorFlag;
extern osEventFlagsId_t motorFlag;
extern osEventFlagsId_t audioFlag;
extern osEventFlagsId_t ultrasonicFlag;

extern osMessageQueueId_t motorMsg;
extern osMessageQueueId_t uartMsg;
extern osMessageQueueId_t ultrasonicMsg;

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

#endif
