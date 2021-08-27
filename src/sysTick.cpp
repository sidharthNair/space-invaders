#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "sysTick.h"
void Systick_Init(uint32_t period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 7;
}
