#include "MKL25Z4.h"                    // Device header
unsigned int counter = 0;
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define SW_POS (6) // PortD Pin 6
#define MASK(x) (1 << (x))

typedef enum {
  green,
  red,
  blue
} color_t;

void initLED(void)
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

void PORTD_IRQHandler()
{
	// Clear Pending IRQ
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	// Updating some variable / flag
	if ((PORTD->ISFR & MASK(SW_POS))) {
		counter++;
	}
	//Clear INT Flag
	PORTD->ISFR = 0xffffffff;
}

void initSwitch(void)
{
    //Enable Clock for PortD
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  
  PORTD->PCR[SW_POS] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1) |
                        PORT_PCR_PE_MASK |
                        PORT_PCR_PS_MASK |
                        PORT_PCR_IRQC(0x0a));
  
  PTD->PDDR &= ~MASK(SW_POS);
  
  NVIC_SetPriority(PORTD_IRQn, 2);
  NVIC_ClearPendingIRQ(PORTD_IRQn);
  NVIC_EnableIRQ(PORTD_IRQn);
  
}

/* MAIN function */
int main(void) 
{
  SystemCoreClockUpdate();
  initLED();
	initSwitch();
  color_t color = red;
  while(1)
  {
		if(counter > 0x02) {
      counter = 0;
    }
    if(counter == 0x00) {
      color = red;
    } else if(counter == 0x01) {
      color = blue;
    } else {
      color = green;
    }
    led_control(color);
		delay(400000);
  }
}