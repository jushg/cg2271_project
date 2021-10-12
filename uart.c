#include "MKL25Z4.h" 

uint8_t UART2_Receive_Poll(void){
 while(!(UART2->S1 & UART_S1_RDRF_MASK));
 return (UART2->D);
}
