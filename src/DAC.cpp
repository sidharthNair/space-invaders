// put implementations for functions, explain how it works
// put your names here, date
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
void DAC_Out(uint8_t value){
	GPIO_PORTB_DATA_R = value;
}
#include "DAC.h"
