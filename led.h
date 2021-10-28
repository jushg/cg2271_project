#include "MKL25Z4.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#define SW_POS 6
#define GREEN_LED_01 7 // PortC Pin 7
#define GREEN_LED_02 0 // PortC Pin 0
#define GREEN_LED_03 3 // PortC Pin 3
#define GREEN_LED_04 4 // PortC Pin 4 
#define GREEN_LED_05 5 // PortC Pin 5
#define GREEN_LED_06 6 // PortC Pin 6
#define GREEN_LED_07 10  // PortC Pin 10
#define GREEN_LED_08 11 // PortC Pin 11

#define RED_LED_01 1 // PortA Pin 1
#define MASK(x) (1 << (x))

void offGreenLEDs(void);

void allGreenLightUp(void);

void allRedLightUp(void);

void offRedLEDs(void);

void toggleGreenLED(int greenLEDCounter);

void flashRedLEDs (int time);
	
void flashGreenLEDs(int time);
