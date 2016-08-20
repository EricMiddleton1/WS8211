#include <msp430.h> 
#include <stdint.h>

#include "WS8211.h"
/*
 * main.c
 */

#define OUT_PIN		BIT4

#define LED_COUNT	100

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    //Configure system clock for 16mhz operation
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    BCSCTL2 = 0;
    BCSCTL3 = 0;

    //Configure output pin
    P1DIR |= OUT_PIN;
    P1OUT &= ~OUT_PIN;

    //Initialize color array
    uint8_t colors[3 * LED_COUNT];
    WS8211_t strip;

    ws8211_init(&strip, colors, LED_COUNT);

    uint16_t i;
    for(i = 0; i < LED_COUNT; i++) {
    	uint8_t r, g, b;
    	Color_fromWheel(i, &r, &g, &b);

    	ws8211_setPixel(&strip, i, r, g, b);
    }


/*
    while(1) {
    	P1OUT |= OUT_PIN;
    	P1OUT |= OUT_PIN;
    	P1OUT |= OUT_PIN;

    	P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;
    	//P1OUT &= ~OUT_PIN;


    }
*/

    //Total bit time = 1.25us +/- 150ns
    //0 code: 0.35us high, 0.9us low +/- 150ns
    //1 code: 0.9us high, 0.35us low +/- 150ns
    //
    //0.9us ~= 14 cycles at 16mhz
    //0.35us ~= 6 cycles at 16mhz

    //Assembly notes:
    //R4 to R15 general purpose registers

    while(1) {
    	static uint8_t color = 0;
    	uint8_t r, g, b;
    	Color_fromWheel(color++, &r, &g, &b);

    	ws8211_shiftForward(&strip);
    	ws8211_setPixel(&strip, 0, r, g, b);

    	ws8211_update(&strip);

    	uint32_t volatile i = 20000;
    	for(; i > 0; --i);
    }

    while(1);


	return 0;
}
