/*----------------------------------------------------------------------------
 * INITIALISATION FUNCTIONS
 *---------------------------------------------------------------------------*/
#include "init.h"

/* INIT CLOCK GATING */
void initClockGate() {
	// Enable Clock Gating for PORTB and PORTE
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTE_MASK) | (SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTD_MASK);
	// Enable Clock Gating for UART2
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		//Enable clock gating for Timer0, Timer 1 and Timer 2
	SIM->SCGC6 = (SIM_SCGC6_TPM0_MASK) | (SIM_SCGC6_TPM1_MASK) | (SIM_SCGC6_TPM2_MASK);
}


/* INITIALISE PWM PINS*/
void initPWM() {
	//Enable Clock Gating for PORTB
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	// Configure Mode 3 for PWM pin operation
	// TPM1_CH0
	PORTB->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_FW] |= PORT_PCR_MUX(3);
	// TPM1_CH1
	PORTB->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_BK] |= PORT_PCR_MUX(3);
	
	// TPM1_CH0
	PORTB->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_FW] |= PORT_PCR_MUX(3);
	// TPM2_CH1
	PORTB->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_BK] |= PORT_PCR_MUX(3);
	
	// Enable Clock gating for Timer 1 and Timer 2
	SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK);
	
	//Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK OR MCGPLLCLK/2
	
	//set modulo value 48000000/128 = 375000, 375000Hz/50Hz = 7500	
	TPM1->MOD = 7500;
	TPM2->MOD = 7500;
	
	//Edge-Aligned PWM
	//CMOD - 1 and PS - 111 (128)
	TPM1_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM1_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
  
	TPM2_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM2_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
	
	//enable PWM on TPM1 channel 0 - PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM1 channel 1 - PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 0 - PTB0
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 1 - PTB1
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

/*	Init UART2	*/
void initUART2(uint32_t baud_rate){
	uint32_t divisor, bus_clock;
    
	// Enable clock for UART2 and Port E
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Enable UART2 receiver on Port 2 pin 23
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	// Disable UART2 transmitter and receiver
	UART2_C2 &= ~(UART_C2_RE_MASK);
	
	// Get default bus clock
	bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
	
	// Get division factor
	divisor = bus_clock / (BAUD_RATE * 16);
	
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2_C1 = 0;
	UART2_S2 = 0;
	UART2_C3 = 0;
	
	NVIC_SetPriority(UART2_IRQn, UART2_INT_PRIO);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	UART2_C2 |= (UART_C2_RE_MASK);
	UART2_C2 |= (UART_C2_RIE_MASK);
}

/* LED GPIO Initialization Function */
void initLED(void) { 
	// Enable Clock to PORTS
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK);
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK);
	
	// Configure MUX settings to make all pins GPIO for GREEN_LED
	PORTC->PCR[GREEN_LED_01] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_01] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_02] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_02] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_03] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_03] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_04] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_04] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_05] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_05] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_06] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_06] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_07] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_07] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_08] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_08] |= PORT_PCR_MUX(1);

	// Configure MUX settings to make all pins GPIO for RED LED
	PORTA->PCR[RED_LED_01] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RED_LED_01] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for PortB and PortD
	PTC->PDDR |= (MASK(GREEN_LED_01) | MASK(GREEN_LED_02) | MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_05) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08));

	PTA->PDDR |= MASK(RED_LED_01);
}

void initAudio() {
	// Configure Mode 3 for PWM pin operation
	PORTD->PCR[PTD0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD0_Pin] |= PORT_PCR_MUX(4);
	PORTD->PCR[PTD1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD1_Pin] |= PORT_PCR_MUX(4);
	
	//Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK OR MCGPLLCLK/2
	
	//set modulo value 48000000/128 = 375000, 375000Hz/50Hz = 7500	
	TPM0->MOD = 7500;
	
	//Edge-Aligned PWM
	//CMOD - 1 and PS - 111 (128)
	TPM0_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM0_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)

	//enable PWM on TPM0 channel 0 - PTD0
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

