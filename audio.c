#include "MKL25Z4.h"                    // Device header
#include "MKL25Z4.h" 
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "init.h"

void buzz(long frequency, uint32_t length) {
	uint32_t mod = 0;
	if(frequency != 0){
		mod = CLOCK_FREQ / PRESCALER / frequency;
	}
	TPM0_MOD = mod;
	TPM0_C0V = mod * DUTY_CYCLE_AUDIO; 
	osDelay(length);
}

void sing(int melody[], int tempo[], int melody_len, uint32_t flag) {
  for (int thisNote = 0; thisNote < melody_len; thisNote++) {
		uint32_t noteDuration = 1000 / tempo[thisNote];
		buzz(melody[thisNote], noteDuration);
		uint32_t pauseBetweenNotes = noteDuration * 1.30;
		osDelay(pauseBetweenNotes);
	}
}


