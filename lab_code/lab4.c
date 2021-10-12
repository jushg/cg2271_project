#include "MKL25Z4.h"
/* Default Core Clk Frequency is 20.97152MHz
  #include CLOCK_SETUP 1 in <system_MKL2574.h> to change it to Core Clock
  
PTB18 -> TM2 CH0
PTB19 -> TM2 CH1
PTB0 -> TM1 CH0
PTB1 -> TM1 CH1
*/

#define PTB0_Pin 0
#define PTB1_Pin 1
#define MUSICAL_NOTE_CNT 7
#define FREQ_2_MOD(x) (375000 / x)

int musical_notes[] = {262, 294, 330, 349, 392, 440, 494};

/* initPWM() */
void initPWM(void)
{
  //Enable Clock Gating for PORTB
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
  
  //Configure Mode 3 for the PWM pin operation (compared to GPIO selection 1)
  PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
  
  PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
  
  //Enable Clock Gating for Timer1
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  
  //Select clock for TPM Module
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK or MCGPLLCLK/2
  
  //Set Modulo Value 20971520 / 128 = 163840 / 3276 = 50Hz
  TPM1->MOD = 3276;
  
  //Set Modulo Value 48000000 / 128 = 375000 / 7500 = 50Hz
  //TPM1->MOD = 7500;
  
  //Edge-Aligned PWM
  //Update SnC resiter: CMOD = 01, PS=111 (128)
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK)); //Clear the bits for CMOD and PS
  TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //Update correct settings
  TPM1->SC &= ~(TPM_SC_CPWMS_MASK); //Clear the bit, operate in up counting mode/edge-aligned
  
  //Enable PWM on TPM1 Channel 0 -> PTB0
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); //High True Pulses
  
  //Enable PWM on TPM1 Channel 1 -> PTB1
  TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); //High True Pulses
}

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void delay_mult100(volatile uint16_t nof){
	for(int i=0; i<100; i++){
		delay(nof);
	}
}

int main(void)
{
  char i = 0;
  SystemCoreClockUpdate();
  initPWM();
  //TPM1_C0V = 0;
  
  //TPM1_C0V = 0x0666; //0x666 = 1638 (half of 3276) -> 50% Duty Cycle
  //TPM1_C0V = 0x0EA6; //0xEA6 = 3750 (half of 7500) -> 50% Duty Cycle
  //TPM1_C1V = 0x753; //0x0EA6
  
  while(1)
  {
    for(i=0;i<MUSICAL_NOTE_CNT;i++)
    {
      TPM1->MOD = FREQ_2_MOD(musical_notes[i]);
      TPM1_C0V = (FREQ_2_MOD(musical_notes[i])) / 2;
			delay_mult100(0xFFFF);
    }
    TPM1_C0V++;
    delay_mult100(0xFFFF);
  }
}
