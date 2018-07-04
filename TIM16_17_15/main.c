#include "gpio.h"						//we use gpio
#include "tim23.h"						//we use tim2->3
#include "tim1615.h"					//we use tim16->15
#include "tim15oc.h"
#include "tim1715.h"

//hardware configuration
#define LED_PORT		GPIOC
#define LED_PIN			(1<<8)			//led on pc8/pc9
#define LED_DLY			500000ul		//time wasted
//end hardware configuration

//global defines

//global variable

//flip the led
void led_flp(void) {
	static uint32_t cnt=1000;
	if (cnt-- == 0) {
		IO_FLP(LED_PORT, LED_PIN);
		cnt = 1000;
	}
}

int main(void) {
	uint32_t time0;
	uint16_t cnt, smcr, cr2, ccmr1;

	mcu_init();

	IO_OUT(LED_PORT, LED_PIN);			//led as output
	tim23_init(10);						//reset tim23
	tim1715_init(10);					//set the prescaler
	//tim15_init(6);
	//tim15_setpr1(10000);
	//tim15_act1(led_flp);
	//ei();
    while(1) {
    	IO_FLP(LED_PORT, LED_PIN);		//blink the led
    	time0=tim1715_get();				//timestamp time0
    	while (tim1715_get() - time0 < LED_DLY) continue;	//wait for time to expire
    	time0=tim1715_get() - time0;
    }
}
