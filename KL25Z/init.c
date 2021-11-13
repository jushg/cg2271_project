/*----------------------------------------------------------------------------
 * INITIALISATION FUNCTIONS
 *---------------------------------------------------------------------------*/
#include "init.h"

/* INIT CLOCK GATING */
void initClockGate() {
	// Enable Clock Gating for PORTB and PORTE
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTE_MASK) | (SIM_SCGC5_PORTA_MASK) 
					| (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK);
	// Enable Clock Gating for UART2
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		//Enable clock gating for Timer0, Timer 1 and Timer 2
	SIM->SCGC6 = (SIM_SCGC6_TPM0_MASK) | (SIM_SCGC6_TPM1_MASK) | (SIM_SCGC6_TPM2_MASK);
}


/*	Init UART2	*/
void initUART2(uint32_t baud_rate){
	uint32_t divisor, bus_clock;
	
	// Enable UART2 receiver on Port E pin 23
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
	PTC->PDDR |= (MASK(GREEN_LED_01) | MASK(GREEN_LED_02) | MASK(GREEN_LED_03) | MASK(GREEN_LED_04) 
	| MASK(GREEN_LED_05) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08));

	PTA->PDDR |= MASK(RED_LED_01);
}

/* Audio Initialization Function */
void initAudio() {
  // configure mode 3 for the pwm pin operation
  PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
  
  // select clock for TPM module
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK or MCGPLLCLK/2
  
  TPM1->MOD = 7500; //when will it overflow
  
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //internal clock and prescaler selection divide by 128
  TPM1->SC &= ~(TPM_SC_CPWMS_MASK); //up counting mode
  
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

/* Motors Initialization Function */
void initMotor() {
	PORTD->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK; 
	PORTD->PCR[LEFT_FW] |= PORT_PCR_MUX(4);

	PORTD->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK; 
	PORTD->PCR[LEFT_BK] |= PORT_PCR_MUX(4);

	PORTD->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK; 
	PORTD->PCR[RIGHT_FW] |= PORT_PCR_MUX(4);

	PORTD->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK; 
	PORTD->PCR[RIGHT_BK] |= PORT_PCR_MUX(4);

	SIM_SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);

	TPM0->MOD = 7500;

	TPM0->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);   //Clearing
	TPM0->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7); 					//Increments in counter clock, 128 ps
	TPM0->SC &= ~TPM_SC_CPWMS_MASK;											

	TPM0_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C0SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);    //Toggle output on match

	TPM0_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C1SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);    //Toggle output on match

	TPM0_C2SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C2SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);    //Toggle output on match

	TPM0_C3SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C3SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);    //Toggle output on match

	//Stop the motors
	LFW = 0;
	LBK = 0;
	RFW = 0;
	RBK = 0;
}  

/* Ultrasonic Sensor Initialization Function */
void initUltrasonic() {
	// Echo Pin Value Reader (ECHO 1) 
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(1);
		
	// Echo Pin Input Capture Timer (ECHO 2)
	PORTB->PCR[PTB2_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB2_Pin] |= PORT_PCR_MUX(3);

	// Trigger Pin (TRIG 1)
	PORTB->PCR[PTB3_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB3_Pin] |= PORT_PCR_MUX(1);

	// Set ECHO 1 and TRIG 1 as GPIO Pins
	PTB->PDDR &= ~MASK(PTB1_Pin);
	PTB->PDDR |= MASK(PTB3_Pin);

	// select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK or MCGPLLCLK/2

	TPM2->MOD = 60000; //when will it overflow

	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));

	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(5)); //internal clock and prescaler selection divide by 32
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK); //up counting mode

	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) | (TPM_CnSC_CHIE_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_ELSB(1) | TPM_CnSC_CHIE(1));  // 

	NVIC_SetPriority(TPM2_IRQn,2);
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_EnableIRQ(TPM2_IRQn);
}
