#ifndef INIT_H
#define INIT_H

#include "util.h"

#define MASK(x) (1 << (x))

#define CLOCK_FREQ 48000000
#define PRESCALER  128
#define PRESCALER_US 1
#define FREQ 50
#define FREQ_US 1000000


// Defining Motors

#define LEFT_FW 1 // PTD1
#define LEFT_BK 3 // PTD3
#define RIGHT_FW 2 // PTB2
#define RIGHT_BK 0 // PTB0


#define LFW TPM0_C1V
#define LBK TPM0_C3V
#define RFW TPM0_C2V
#define RBK TPM0_C0V


#define UART_RX_PORTE23 23 //PTE 23 RX
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128	

// SOUND
#define PTB0_Pin 0
#define DUTY_CYCLE_AUDIO 0.8

#define AUDIO_MOD TPM1_MOD
#define AUDIO_FREQ TPM1_C0V

//LED
#define GREEN_LED_01 7 // PortC Pin 7
#define GREEN_LED_02 0 // PortC Pin 0
#define GREEN_LED_03 3 // PortC Pin 3
#define GREEN_LED_04 4 // PortC Pin 4 
#define GREEN_LED_05 5 // PortC Pin 5
#define GREEN_LED_06 6 // PortC Pin 6
#define GREEN_LED_07 10  // PortC Pin 10
#define GREEN_LED_08 11 // PortC Pin 11

#define RED_LED_01 12 // PortA Pin 12

// ULTRASONIC
#define PTB1_Pin 1 //Echo 1
#define PTB2_Pin 2 //Echo 2
#define PTB3_Pin 3 //Trigger 1

// MISCELLANEOUS MACROS

#define CLEAR_IRQ 0xffffffff
#define INIT_VAR 0xFE


void initClockGate(void);
void initUART2(uint32_t);
void initLED(void);
void initAudio(void);
void initMotor(void);
void initUltrasonic(void);

#endif
