#ifndef AUDIO_H_
#define AUDIO_H_
#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "init.h"

osEventFlagsId_t audioFlag;

#define REST 0
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS 455
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

void buzz(long frequency, uint32_t length) {
	uint32_t mod = 0;
	if(frequency != 0){
		mod = CLOCK_FREQ / PRESCALER / frequency;
	}
	AUDIO_MOD = mod;
	AUDIO_FREQ = mod * DUTY_CYCLE_AUDIO; 
	osDelay(length);
}

void sing(int melody[], int tempo[], int melody_len, uint32_t flag) {
  for (int thisNote = 0; thisNote < melody_len; thisNote++) {
		osEventFlagsWait(audioFlag, flag, osFlagsNoClear, osWaitForever);
		uint32_t noteDuration = 1000 / tempo[thisNote];
		buzz(melody[thisNote], noteDuration);
		uint32_t pauseBetweenNotes = noteDuration * 1.30;
		osDelay(pauseBetweenNotes);
	}
}
int connected_melody[] = {
  NOTE_E6, 0, NOTE_B5, 0
};

int connected_tempo[] = {
  12, 12, 12, 12
};

int birthday[] = {
NOTE_G6,0, NOTE_G6,  NOTE_A6, NOTE_G6, NOTE_C6, NOTE_B6, 0, 
NOTE_G6,0, NOTE_G6 ,NOTE_A6,  NOTE_G6, NOTE_D6, NOTE_C6, 0,
NOTE_G6,0, NOTE_G6, NOTE_G6, NOTE_E6, NOTE_C6, 
NOTE_B6, NOTE_A6, 0, 
NOTE_F6,0, NOTE_F6, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_C6,0
};
int birthday_tempo[] = {
6,10,6,6,6,6,6,6,
6,10,6,6,6,6,6,6,
6,10,6,6,6,6,6,6,6,
6,10,6,6,6,6,6,6
};


int twinkle_stars[] = {
NOTE_C5, 0, NOTE_C5, 0, NOTE_G5, 0, NOTE_G5, 0, NOTE_A5,  0, NOTE_A5, 0, NOTE_G5, 0,
NOTE_F5, 0, NOTE_F5, 0, NOTE_E5, 0, NOTE_E5, 0, NOTE_D5, 0, NOTE_D5, 0, NOTE_C5, 0,
NOTE_G5, 0, NOTE_G5, 0,  NOTE_F5, 0, NOTE_F5, 0, NOTE_E5, 0, NOTE_E5, 0, NOTE_D5, 0,
NOTE_G5, 0, NOTE_G5, 0, NOTE_F5, 0, NOTE_F5, 0, NOTE_E5, 0, NOTE_E5, 0, NOTE_D5, 0,
NOTE_C5, 0, NOTE_G5, 0, NOTE_G5, 0, NOTE_A5, 0, NOTE_A5, 0, NOTE_G5, 0,
NOTE_F5, 0, NOTE_F5, 0, NOTE_E5, 0, NOTE_E5, 0, NOTE_D5, 0, NOTE_C5, 0
};

int twinkle_stars_tempo[] = {
6,8,6,8,6,8,6,8,6,8,6,8,6,10,
6,8,6,8,6,8,6,8,6,8,6,8,6,10,
6,8,6,8,6,8,6,8,6,8,6,8,6,10,
6,8,6,8,6,8,6,8,6,8,6,8,6,10,
6,8,8,8,6,8,6,8,6,8,6,8,6,10,
6,8,6,8,6,8,6,8,6,8,6,10
};



int gurenge[] = {
	NOTE_G6,NOTE_FS6,NOTE_G6,0,
	NOTE_G6,NOTE_FS6,NOTE_G6,0,
	NOTE_G6,NOTE_FS6,NOTE_E6,0,
	NOTE_D6,0,
	NOTE_B5,NOTE_D6,NOTE_E6,0,
	NOTE_E6,NOTE_G6,NOTE_A6,0,
	NOTE_G6,NOTE_A6,NOTE_B6,0,
	
	NOTE_D6,0,NOTE_D6,NOTE_B6,
	NOTE_A6,NOTE_B6,NOTE_D7,
	NOTE_B6,NOTE_A6,NOTE_B6,0,NOTE_B6,
	NOTE_G6,NOTE_A6,NOTE_B6,0,NOTE_D7,
	NOTE_B6,NOTE_A6,NOTE_B6,NOTE_G6,0,
	NOTE_G6,NOTE_G6,NOTE_A6,NOTE_B6,NOTE_G6,
	NOTE_G7,NOTE_FS7,0
	
};
	

//Mario main theme melody
int mario_melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int gurenge_tempo[] = {
	6,6,4,5,
	6,6,4,5,
	6,6,4,7,
	4,4,
	6,6,4,5,
	6,6,4,5,
	6,6,4,3,
	
	4,48,4,4,
	8,8,8,
	8,8,16,64,4,
	8,8,16,64,8,
	8,8,8,4,64,
	16,16,8,4,6,
	8,8,64
};
//Mario main them tempo
int mario_tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

#endif
