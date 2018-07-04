#include "tim1615.h"						//we use 32-bit timer 16 / LSW -> timer15 / MSW

//hardware configuration
//end hardware configuration

//global defines

//global variable


//initialize TIM1615 as a synchronized 32-bit timer
//tim16 as master / lsw (including prescaler) to tim15 / msw of the 32-bit timer
//tim15 as slave / msw of the 32-bit timer
void tim1615_init(uint16_t ps) {
	//initialize TIM16 as master
	//enable clock to TIM16
	RCC->APB2ENR |= 	RCC_APB2ENR_TIM16EN |
					0x00;
	//stop the timer to configure it
	TIM16->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM16->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM16->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM16->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier
	//CR2 on TIM16/17 doesn't exist -> the following statement has no effect
	//thus it doesn't work
	TIM16->CR2 = 0x20;					//MMS = 0b010->TIM16 as prescaler
	//alternative: set up OC1 as the TRGO signal
	TIM16->CCER&=~TIM_CCER_CC1E;		//0->disable cc1, 1->enable cc1
	//disable cc1e to change ccmr1
	//TIM16->CCMR1&=~(0x03<<0);			//0->CC1 as output
	TIM16->CCMR1= 0x06<<4;				//1->oc1ref high on match, 3->oc1ref flip on match, 6->pwm mode 1(
	TIM16->CCER|= TIM_CCER_CC1E;		//0->disable cc1, 1->enable cc1
	//TIM16->EGR = TIM_EGR_UG | TIM_EGR_CC1G;			//force an update
	TIM16->CCR1= 0xffff;				//set the match point: anything but 0
	TIM16->BDTR|= TIM_BDTR_MOE;			//1->set MOE

	//source from internal clock -> disable slave mode
	TIM16->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM16->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM16->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM16->PSC = ps - 1;					//set the prescaler to ps
	TIM16->RCR = 0;						//repetition counter = 0 (=no repetition)
	//user can specify a prescaler here. otherwise use 0xffff
	TIM16->ARR = 0xffff;					//auto reload register / period = 0; - need to change for downcounters
	TIM16->CNT = 0;						//reset the counter

	//enable the timer.
	//TIM16->CR1 |= TIM_CR1_CEN;			//enable the timer

	//initialize TIM15 as slave
	RCC->APB2ENR |= 	RCC_APB2ENR_TIM15EN |
					0x00;
	//stop the timer to configure it
	TIM15->CR1 &=~TIM_CR1_CEN;			//clear cen. 0=disable the timer, 1=enable the timer
	TIM15->CR1 &=~TIM_CR1_CKD;			//clear CKD0..1. 0b00->1x clock; 0b01->2:1 clock, 0b10->4:1 clk; 0b11->reserved
	TIM15->CR1 &=~TIM_CR1_DIR;			//clear DIR bit. 0=upcounter, 1=downcounter
	TIM15->CR1 &=~TIM_CR1_OPM;			//clear opm bit. 0=periodic timer, 1=one-shot timer
	//or to simply zero the register
	//TIMx->CR1 = 0;						//much easier

	//source from internal clock -> disable slave mode
	TIM15->SMCR &=~TIM_SMCR_SMS;			//clear sms->master mode and use internal clock

	//clear the status register bits for capture / compare flags
	TIM15->SR &=~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF | TIM_SR_UIF);
	//disable the interrupt by clearing the enable bits
	TIM15->DIER &=~(TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

	//set the prescaler
	TIM15->PSC = 0;						//set the prescaler to 1:1 - master timer acts as prescaler
	TIM15->RCR = 0;						//repetition counter = 0 (=no repetition)
	TIM15->ARR = 0xffff;				//auto reload register / period = 0; - need to change for downcounters
	TIM15->CNT = 0;						//reset the counter
	TIM15->EGR  = TIM_EGR_UG;			//force an update

	//source from trgo -> enable slave mode and trigger on trgo
	TIM15->SMCR = (TIM15->SMCR &~((0x07 << 4) | (0x07 << 0))) |
			(0x02 << 4) | 				//tab 74: 0b010->//slave TIM15 driven by TIM16
			(0x07 << 0) |				//0b111->external trigger on trgo
			(1<<7) |					//set sms
			0x00;

	//enable the timer.
	TIM15->CR1 |= TIM_CR1_CEN;			//enable the timer
	//enable the timer.
	TIM16->CR1 |= TIM_CR1_CEN;			//enable the timer
}

//read 32-bit timer value
uint32_t tim1615_get(void) {
	uint16_t msw, lsw;					//timer's high/low words

	//double read to maintain atomicity
	do {
		msw = TIM15->CNT;				//read the msw
		lsw = TIM16->CNT;				//read the lsw
	} while (msw != TIM15->CNT);			//see if overflow has taken place
	return ((uint32_t) msw << 16) | lsw;			//return 32-bit time
}

