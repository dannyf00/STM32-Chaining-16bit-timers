#ifndef _TIM1615_H
#define _TIM1615_H

#include "gpio.h"						//we use gpio

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize tim23 as a synchronized 32-bit timer
//tim16 as master / lsw (including prescaler) to tim15 / msw of the 32-bit timer
//tim15 as slave / msw of the 32-bit timer
void tim1615_init(uint16_t ps);

//read 32-bit timer value
uint32_t tim1615_get(void);

#endif
