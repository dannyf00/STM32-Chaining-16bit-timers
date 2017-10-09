//construct a 32-bit timer by synchronizing two 16-bit timers
//tim2 as master and tim3 as slave

#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
#include "coretick.h"				//we use dwt
#include "systick.h"				//we use systick

//hardware configuration
#define LEDG_PORT		GPIOC
#define LEDG			(1<<9)		//green led on PC9 on Discovery board

#define LEDB_PORT		GPIOC
#define LEDB			(1<<8)		//blue led on PC8 on Discovery board

#define LED_PORT		LEDG_PORT
#define LED				LEDG
//end hardware configuration

//global defines

//global variables

//initialize tim23 as a synchronized 32-bit timer
//tim2 as master / prescaler to tim3 / lsw of the 32-bit timer
//tim3 as slave / msw of the 32-bit timer
void tim23_init(uint16_t ps) {
	//initialize tim2 as master
	//enable clock to tim2
	RCC->APB1ENR |= 	RCC_APB1ENR_TIM2EN |
					0x00;
	//stop the timer to configure it
	TIM2->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM2->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM2->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM2->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier
	TIM2->CR2 = 0x20;					//MMS = 0b010->tim2 as prescaler

	//source from internal clock -> disable slave mode
	TIM2->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM2->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM2->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM2->PSC = ps - 1;					//set the prescaler to ps
	TIM2->RCR = 0;						//repetition counter = 0 (=no repetition)
	//user can specify a prescaler here. otherwise use 0xffff
	TIM2->ARR = 0xffff;					//auto reload register / period = 0; - need to change for downcounters
	TIM2->CNT = 0;						//reset the counter

	//enable the timer.
	TIM2->CR1 |= TIM_CR1_CEN;			//enable the timer

	//initialize tim3 as slave
	RCC->APB1ENR |= 	RCC_APB1ENR_TIM3EN |
					0x00;
	//stop the timer to configure it
	TIM3->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM3->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM3->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM3->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier

	//source from internal clock -> disable slave mode
	TIM3->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM3->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM3->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM3->PSC = 0;						//set the prescaler to 1:1 - master timer acts as prescaler
	TIM3->RCR = 0;						//repetition counter = 0 (=no repetition)
	TIM3->ARR = 0xffff;					//auto reload register / period = 0; - need to change for downcounters
	TIM3->CNT = 0;						//reset the counter

	//enable the timer.
	TIM3->CR1 |= TIM_CR1_CEN;			//enable the timer

	//source from trgo -> enable slave mode and trigger on trgo
	TIM3->SMCR = (TIM3->SMCR &~((0x07 << 4) | 0x07)) |
			(0x01 << 4) | 				//tab 71: 0b001->//slave tim3 driven by tim2
			(0x07 << 0) |				//0b111->external trigger on trgo
			0x00;
}

uint32_t tmr23_get(void) {
	uint16_t msw, lsw;					//timer's high/low words

	//double read to maintain atomicity
	do {
		msw = TIM3->CNT;				//read the msw
		lsw = TIM2->CNT;				//read the lsw
	} while (msw != TIM3->CNT);			//see if overflow has taken place
	return (msw << 16) | lsw;			//return 32-bit time
}

uint32_t time0, time1;
int32_t tmp;
int main(void) {
	int i;

	mcu_init();									//reset the  mcu
	coretick_init();							//reset coretick

	//initialize the pins to outout
	IO_OUT(LEDG_PORT, LEDG);					//ledg/b as output
	IO_OUT(LEDB_PORT, LEDB);

	tim23_init(2);								//initialize 32-bit timer from tim2/3, prescaler = 2

	ei();										//enable global interrupts
	while (1) {
		//test: flip a pin every 2 * (1<<22) clock cycles have been reached
		if (tmr23_get() & (1ul<<22)) IO_SET(LEDG_PORT, LEDG); else IO_CLR(LEDG_PORT, LEDG);
	};
}
