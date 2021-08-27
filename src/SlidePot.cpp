// SlidePot.cpp
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "SlidePot.h"
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
SYSCTL_RCGCADC_R |= 0x1;
SYSCTL_RCGCGPIO_R |= 0x8;
__nop();
__nop();
__nop();
GPIO_PORTD_DIR_R &= ~0x4;
GPIO_PORTD_AFSEL_R |= 0x4;
GPIO_PORTD_DEN_R &= ~0x4;
GPIO_PORTD_AMSEL_R |= 0x4;
__nop();
__nop();
ADC0_PC_R &= ~0xF;
ADC0_PC_R |= 0x1;
ADC0_SSPRI_R = 0x0123;
ADC0_ACTSS_R &= ~0x8;
ADC0_EMUX_R &= ~0xF000;
ADC0_SSMUX3_R &= ~0xF;
ADC0_SSMUX3_R += 5;
ADC0_SSCTL3_R = 0x06;
ADC0_IM_R &= ~0x8;
ADC0_SAC_R = ADC_SAC_AVG_32X;
ADC0_ACTSS_R |= 0x8;
}

//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t result;
	ADC0_PSSI_R = 0x08;
	while((ADC0_RIS_R & 0x08) == 0){
		__nop();
	}
	result = ADC0_SSFIFO3_R & 0xFFF;
	ADC0_ISC_R = 0x08;
  return result;
}
int8_t getMovementDirection(){
	uint32_t ADCResult = ADC_In();
	if(ADCResult <= 1365)
	{
		return -1; //player wants to move left
	}
	else if(ADCResult >=2730)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

// constructor, invoked on creation of class
// m and b are linear calibration coeffients 
SlidePot::SlidePot(uint32_t m, uint32_t b){
// lab 8
}

void SlidePot::Save(uint32_t n){
// lab 9
}
uint32_t SlidePot::Convert(uint32_t n){
  // lab 8
  return 0;
}

void SlidePot::Sync(void){
// lab 8
}

uint32_t SlidePot::ADCsample(void){ // return ADC sample value (0 to 4095)

  return 0;
}

uint32_t SlidePot::Distance(void){  // return distance value (0 to 2000), 0.001cm
// lab 8
  return 0;
}
