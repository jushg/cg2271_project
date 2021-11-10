#include "led.h"

void offGreenLEDs() {
	PTC->PDOR = ~(MASK(GREEN_LED_01) | MASK(GREEN_LED_02) | MASK(GREEN_LED_03) | MASK(GREEN_LED_04) 
	| MASK(GREEN_LED_05) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08));
}

void allGreenLightUp() {
		PTC->PDOR = (MASK(GREEN_LED_01) | MASK(GREEN_LED_02) | MASK(GREEN_LED_03) | MASK(GREEN_LED_04) 
	| MASK(GREEN_LED_05) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08));
}

void flashGreenLEDs(int time){
		allGreenLightUp();
		osDelay(time);
		offGreenLEDs();
		osDelay(time);
}

void flashRedLEDs(int time){
		allRedLightUp();
		osDelay(time);
		offRedLEDs();
		osDelay(time);
}

void allRedLightUp() {
		PTA->PDOR = MASK(RED_LED_01);
}

void offRedLEDs() {
		PTA->PDOR = ~MASK(RED_LED_01);
}

void toggleGreenLED(int greenLEDCounter) {	
	offGreenLEDs();
	int greenLedArray[8] = {MASK(GREEN_LED_01), MASK(GREEN_LED_02), MASK(GREEN_LED_03), MASK(GREEN_LED_04), 
					MASK(GREEN_LED_05), MASK(GREEN_LED_06), MASK(GREEN_LED_07), MASK(GREEN_LED_08)};
	PTC->PDOR = greenLedArray[greenLEDCounter];
} 
