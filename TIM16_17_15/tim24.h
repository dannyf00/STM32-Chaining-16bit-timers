#ifndef _TIM24_H
#define _TIM24_H

#include "gpio.h"						//we use gpio

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize tim23 as a synchronized 32-bit timer
//tim2 as master / lsw (including prescaler) to tim3 / msw of the 32-bit timer
//tim3 as slave / msw of the 32-bit timer
void tim24_init(uint16_t ps);

//read 32-bit timer value
uint32_t tim24_get(void);

#endif
