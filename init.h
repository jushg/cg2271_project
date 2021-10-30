#ifndef INIT_H
#define INIT_H
#include "MKL25Z4.h"                    // Device header
#include <math.h>
#include "led.h"
#define MASK(x) (1 << (x))

#define CLOCK_FREQ 48000000
#define PRESCALER  128
#define PRESCALER_US 1
#define FREQ 50
#define FREQ_US 1000000

//define Motors 2
/*
#define LEFT_FW 0 // PTB0 TPM1_CH0
#define LEFT_BK 1 // PTB1 TPM1_CH1
#define RIGHT_FW 2 // PTB2 TPM2_CH0
#define RIGHT_BK 3 // PTB3 TPM2_CH1
*/

#define LFW TPM0_C1V
#define LBK TPM0_C3V
#define RFW TPM0_C2V
#define RBK TPM0_C0V

// Defining Motors

#define LEFT_FW 1 // PTB0 TPM1_CH0
#define LEFT_BK 3 // PTB1 TPM1_CH1
#define RIGHT_FW 2 // PTB2 TPM2_CH0
#define RIGHT_BK 0 // PTB3 TPM2_CH1


#define UART_RX_PORTE23 23 //PTE 23 RX
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128	

// SOUND
#define PTB0_Pin 0

//#define PTD0_Pin 0
//#define PTD1_Pin 1
#define DUTY_CYCLE_AUDIO 0.8

#define AUDIO_MOD TPM1_MOD
#define AUDIO_FREQ TPM1_C0V


// ULTRASONIC
#define PTB1_Pin 1
#define PTB2_Pin 2
#define PTB3_Pin 3

// MISCELLANEOUS MACROS


#define CLEAR_IRQ 0xffffffff
#define INIT_VAR 0xFE
#define RESET 0x00



void initClockGate(void);
void initPWM(void);
void initUART2(uint32_t);
void initLED(void);
void initAudio(void);
void initUltrasonic(void);
void initMotor(void);

#endif
