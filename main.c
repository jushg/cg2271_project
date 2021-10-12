/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
typedef enum {
  green,
  red,
  blue
} color_t;

void InitGPIO(void)
{
// Enable Clock to PORTB and PORTD
SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
// Configure MUX settings to make all 3 pins GPIO
PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
// Set Data Direction Registers for PortB and PortD
PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
PTD->PDDR |= MASK(BLUE_LED);
}

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void clearRGB(void) {
  PTB->PDOR = (MASK(RED_LED) | MASK(GREEN_LED));
  PTD->PDOR = MASK(BLUE_LED);
}

static void led_control(color_t color) {
  clearRGB();
  if(color == green) {
    PTB->PDOR = ~MASK(GREEN_LED);
    //PTD->PDOR = MASK(BLUE_LED);
  } else if (color == red) {
    PTB->PDOR = ~MASK(RED_LED);
    //PTD->PDOR = MASK(BLUE_LED);
  } else {
    //PTB->PDOR = MASK(RED_LED) || MASK(GREEN_LED);
    PTD->PDOR = ~MASK(BLUE_LED);
  }
}

void led_red_thread (void *argument) {
 
  // ...
	color_t color = red;
  for (;;) {
		led_control (color);
		//osDelay(1000);
		delay(0x80000);
		//led_control(color);
		clearRGB();
		//osDelay(1000);
		delay(0x80000);
	}
}

void led_green_thread (void *argument) {
 
  // ...
	color_t color = green;
  for (;;) {
		led_control (color);
		osDelay(1000);
		//delay(0x80000); //
		//led_control(color);
		clearRGB();
		osDelay(1000);//delay(0x80000); //
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	clearRGB();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(led_red_thread, NULL, NULL);    // Create application main thread
  osThreadNew(led_green_thread, NULL, NULL);
	osKernelStart();                      // Start thread execution
  for (;;) {}
}
