#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "initializations.h"

void generalIOInit(void){
	SYSCTL_RCGCGPIO_R |= 0x12; //Clock for ports B, E (port A assumed enabled)
	__nop();
	__nop();
	
	GPIO_PORTB_DIR_R |= 0x3F; //initalize DAC
	GPIO_PORTB_DEN_R |= 0x3F;
	

	GPIO_PORTE_DIR_R |= 0xC; //initialize button inputs and LED outputs
	GPIO_PORTE_DIR_R &= ~0x3;
	GPIO_PORTE_DEN_R |= 0xF;
	GPIO_PORTE_IS_R &= ~0x3; //PE0 and 1 edge sensitive
	GPIO_PORTE_IBE_R &= ~0x3; //Not both edges
	GPIO_PORTE_IEV_R |= 0x3; //rising edge event
	GPIO_PORTE_ICR_R = 0x3; //clear triggers
	GPIO_PORTE_IM_R |= 0x3; //arm interrupts on both buttons
	NVIC_PRI1_R = (NVIC_PRI1_R & ~0xE0) | 0x40; //priority 2 buttons
	NVIC_EN0_R |= 0x10; //enable interrupt number 4
}
void SysTick_Init(){
  //*** students write this ******
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 1599999; //50Hz
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 7;
}

