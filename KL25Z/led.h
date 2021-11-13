#include "MKL25Z4.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "init.h"

void offGreenLEDs(void);
void allGreenLightUp(void);
void allRedLightUp(void);
void offRedLEDs(void);
void toggleGreenLED(int greenLEDCounter);
void flashRedLEDs (int time);
void flashGreenLEDs(int time);
