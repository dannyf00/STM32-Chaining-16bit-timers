#include "gpio.h"					//we use aruidno port for stm8s

//global definitions

//global variables
//configure gpio DDR mode (cnf1..0 + mod1..0 bits)
void GPIO_DDR(GPIO_TypeDef * gpio, uint32_t mask, uint32_t mode) {
	mode = mode & 0x0f;				//lowest 4 bits effective

	//for the lowest 8 bits
	if (mask & (1ul<< 0)) gpio->CRL = (gpio->CRL &~0x0000000ful) | (mode << (0 * 4));
	if (mask & (1ul<< 1)) gpio->CRL = (gpio->CRL &~0x000000f0ul) | (mode << (1 * 4));
	if (mask & (1ul<< 2)) gpio->CRL = (gpio->CRL &~0x00000f00ul) | (mode << (2 * 4));
	if (mask & (1ul<< 3)) gpio->CRL = (gpio->CRL &~0x0000f000ul) | (mode << (3 * 4));
	if (mask & (1ul<< 4)) gpio->CRL = (gpio->CRL &~0x000f0000ul) | (mode << (4 * 4));
	if (mask & (1ul<< 5)) gpio->CRL = (gpio->CRL &~0x00f00000ul) | (mode << (5 * 4));
	if (mask & (1ul<< 6)) gpio->CRL = (gpio->CRL &~0x0f000000ul) | (mode << (6 * 4));
	if (mask & (1ul<< 7)) gpio->CRL = (gpio->CRL &~0xf0000000ul) | (mode << (7 * 4));

	//for the highest 8 bits
	if (mask & (1ul<< 8)) gpio->CRH = (gpio->CRH &~0x0000000ful) | (mode << (0 * 4));
	if (mask & (1ul<< 9)) gpio->CRH = (gpio->CRH &~0x000000f0ul) | (mode << (1 * 4));
	if (mask & (1ul<<10)) gpio->CRH = (gpio->CRH &~0x00000f00ul) | (mode << (2 * 4));
	if (mask & (1ul<<11)) gpio->CRH = (gpio->CRH &~0x0000f000ul) | (mode << (3 * 4));
	if (mask & (1ul<<12)) gpio->CRH = (gpio->CRH &~0x000f0000ul) | (mode << (4 * 4));
	if (mask & (1ul<<13)) gpio->CRH = (gpio->CRH &~0x00f00000ul) | (mode << (5 * 4));
	if (mask & (1ul<<14)) gpio->CRH = (gpio->CRH &~0x0f000000ul) | (mode << (6 * 4));
	if (mask & (1ul<<15)) gpio->CRH = (gpio->CRH &~0xf0000000ul) | (mode << (7 * 4));

}

//reset the chip
void mcu_init(void) {
	//select the clock source
	//or use default clock

	//enable clock to GPIO
	RCC->APB2ENR |=
			RCC_APB2ENR_IOPAEN |
			RCC_APB2ENR_IOPBEN |
			RCC_APB2ENR_IOPCEN |
			RCC_APB2ENR_IOPDEN |
			RCC_APB2ENR_IOPEEN |
#if defined(RCC_APB2ENR_IOPFEN)
			RCC_APB2ENR_IOPFEN |
#endif
#if defined(RCC_APB2ENR_IOPGEN)
			RCC_APB2ENR_IOPGEN |
#endif
			0x00;

	//update SystemCoreClock - it must be the last step before exiting mcu_init
	SystemCoreClockUpdate();
}
