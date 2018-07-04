#ifndef _TIM234_H
#define _TIM234_H

#include "gpio.h"						//we use gpio

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize tim2/3/4 as a synchronized 48-bit timer
//tim2 as master / lsw (including prescaler) to tim3 / msw of the 48-bit timer
//tim3 as slave/master / middle word of the 48-bit timer
//tim4 as slave / msw word of the 48-bit timer
void tim234_init(uint16_t ps);

//read 48-bit timer value
uint64_t tim234_get(void);

#endif
