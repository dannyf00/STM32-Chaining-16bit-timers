#include "tim234.h"						//we use 48-bit timer 2 / LSW -> timer3 / MidW -> timer4 / MSW

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize tim2/3/4 as a synchronized 32-bit timer
//tim2 as master / lsw (including prescaler) to tim3 / msw of the 48-bit timer
//tim3 as slave/master / middle word of the 48-bit timer
//tim4 as slave / msw word of the 48-bit timer
void tim234_init(uint16_t ps) {
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
	//TIM2->CR1 |= TIM_CR1_CEN;			//enable the timer

	//enable clock to tim3
	RCC->APB1ENR |= 	RCC_APB1ENR_TIM3EN |
					0x00;
	//stop the timer to configure it
	TIM3->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM3->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM3->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM3->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier
	TIM3->CR2 = 0x20;					//MMS = 0b010->tim2 as prescaler

	//source from internal clock -> disable slave mode
	TIM3->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM3->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM3->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM3->PSC = 0;						//set the prescaler to ps
	TIM3->RCR = 0;						//repetition counter = 0 (=no repetition)
	//user can specify a prescaler here. otherwise use 0xffff
	TIM3->ARR = 0xffff;					//auto reload register / period = 0; - need to change for downcounters
	TIM3->CNT = 0;						//reset the counter

	//source from trgo -> enable slave mode and trigger on trgo
	TIM3->SMCR = (TIM3->SMCR &~((0x07 << 4) | (0x07 << 0))) |
			(0x01 << 4) | 				//tab 71: 0b001->//slave tim3 driven by tim2
			(0x07 << 0) |				//0b111->external trigger on trgo
			0x00;

	//enable the timer.
	//TIM2->CR1 |= TIM_CR1_CEN;			//enable the timer


	//initialize tim4 as slave
	RCC->APB1ENR |= 	RCC_APB1ENR_TIM4EN |
					0x00;
	//stop the timer to configure it
	TIM4->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM4->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM4->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM4->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier

	//source from internal clock -> disable slave mode
	TIM4->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM4->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM4->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM4->PSC = 0;						//set the prescaler to 1:1 - master timer acts as prescaler
	TIM4->RCR = 0;						//repetition counter = 0 (=no repetition)
	TIM4->ARR = 0xffff;					//auto reload register / period = 0; - need to change for downcounters
	TIM4->CNT = 0;						//reset the counter

	//enable the timer.
	//TIM4->CR1 |= TIM_CR1_CEN;			//enable the timer

	//source from trgo -> enable slave mode and trigger on trgo
	TIM4->SMCR = (TIM4->SMCR &~((0x07 << 4) | (0x07 << 0))) |
			(0x02 << 4) | 				//tab 71: 0b010->//slave tim4 driven by tim3
			(0x07 << 0) |				//0b111->external trigger on trgo
			0x00;

	//enable the timer: MSW first, and LSW last
	TIM4->CR1 |= TIM_CR1_CEN;			//enable the timer
	TIM3->CR1 |= TIM_CR1_CEN;			//enable the timer
	TIM2->CR1 |= TIM_CR1_CEN;			//enable the timer
}

//read 48-bit timer value
uint64_t tim234_get(void) {
	uint16_t msw, midw, lsw;					//timer's high/low words

	//double read to maintain atomicity
	do {
		msw = TIM4->CNT;				//read the msw
		midw= TIM3->CNT;
		lsw = TIM2->CNT;				//read the lsw
	} while ((msw != TIM4->CNT) | (midw != TIM3->CNT));			//see if overflow has taken place
	return ((uint64_t) msw << 32) | ((uint32_t) midw << 16) | lsw;			//return 32-bit time
}

