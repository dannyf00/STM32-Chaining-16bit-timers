#ifndef _TIM1715_H
#define _TIM1715_H

#include "gpio.h"						//we use gpio

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize tim23 as a synchronized 32-bit timer
//tim17 as master / lsw (including prescaler) to tim15 / msw of the 32-bit timer
//tim15 as slave / msw of the 32-bit timer
void tim1715_init(uint16_t ps);

//read 32-bit timer value
uint32_t tim1715_get(void);

#endif
