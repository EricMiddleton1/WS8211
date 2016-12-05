#include <msp430.h> 
#include <stdint.h>

#include "WS8211.h"
#include "StrangerThings.h"
#include "Sparkle.h"
#include "Timer.h"
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

    Timer_init();

    //Configure output pin
    P1DIR |= OUT_PIN;
    P1OUT &= ~OUT_PIN;

    //Initialize color array
    uint8_t colors[3 * LED_COUNT];
    WS8211_t strip;
    SparkleEffect sparkleEffect;

    ws8211_init(&strip, colors, LED_COUNT);
    Sparkle_init(&sparkleEffect, 20, LED_COUNT);

    while(1) {
    	Sparkle_update(&sparkleEffect, &strip);

    	//Timer_waitMillis(10);
    }


   /*
    const char* msg = "SEND NUDES ";
    unsigned int msgLen = strlen(msg);

    while(1) {
    	static unsigned int i = 0;

    	StrangerThings_display(&strip, msg, i++, 64);

    	if(i >= msgLen) {
    		i = 0;
    	}

    	//Delay
    	uint32_t volatile j = 1000000;
    	for(; j > 0; --j);
    }

    uint16_t i;
    for(i = 0; i < LED_COUNT; i++) {
    	uint8_t r, g, b;
    	Color_fromWheel(i, &r, &g, &b);

    	ws8211_setPixel(&strip, i, r, g, b);
    }
*/

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

volatile uint8_t cnt = 0;

#pragma vector=PORT2_VECTOR
#pragma vector=PORT1_VECTOR
#pragma vector=TIMER1_A1_VECTOR
#pragma vector=TIMER1_A0_VECTOR
#pragma vector=TIMER0_A0_VECTOR
#pragma vector=ADC10_VECTOR
#pragma vector=USCIAB0TX_VECTOR
#pragma vector=WDT_VECTOR
#pragma vector=USCIAB0RX_VECTOR
#pragma vector=NMI_VECTOR
#pragma vector=COMPARATORA_VECTOR
__interrupt void TRAPINT_ISR(void)

{
	cnt++;
}
