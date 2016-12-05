/*
 * WS8211.c
 *
 *  Created on: Aug 20, 2016
 *      Author: eric
 */

#include <msp430.h>

#include "WS8211.h"

void __ws8211_update(uint8_t colors[], uint16_t count);

void ws8211_init(WS8211_t* strip, uint8_t colors[], uint16_t size) {
	//Initialize struct
	strip->colors = colors;
	strip->size = 3*size; //strip->size represents total colors (=3*total pixels)

	//Clear pixels
	//memset(colors, 0, strip->size);
	for(size = strip->size; size > 0; --size) {
		colors[size-1] = 0x00;
	}

	//Update
	__ws8211_update(strip->colors, strip->size);
}

void ws8211_clear(WS8211_t* strip) {
	uint16_t i;
	for(i = 0; i < strip->size; ++i) {
		strip->colors[i] = 0x00;
	}
}

void ws8211_setPixel(WS8211_t* strip, uint16_t id, uint8_t r, uint8_t g, uint8_t b) {
	//Calculate actual color offset
	id *= 3;

	//Colors stored in GRB order
	strip->colors[id] = r;
	strip->colors[id + 1] = g;
	strip->colors[id + 2] = b;
}

void ws8211_update(WS8211_t* strip) {
	__ws8211_update(strip->colors, strip->size);
}

void ws8211_getPixel(WS8211_t* strip, uint16_t id, uint8_t* r, uint8_t* g, uint8_t* b) {
	//Calculate actual color offset
	id *= 3;

	//Colors stored in GRB order
	*r = strip->colors[id];
	*g = strip->colors[id+1];
	*b = strip->colors[id+2];
}

void ws8211_shiftForward(WS8211_t* strip) {
	uint8_t r, g, b;
	uint16_t i;

	ws8211_getPixel(strip, strip->size/3 - 1, &r, &g, &b);

	for(i = strip->size/3 - 1; i > 0; i--) {
		uint8_t r, g, b;
		ws8211_getPixel(strip, i - 1, &r, &g, &b);

		ws8211_setPixel(strip, i, r, g, b);
	}

	ws8211_setPixel(strip, 0, r, g, b);
}

Color Color_rgb(uint8_t r, uint8_t g, uint8_t b) {
	Color c = {r, g, b};

	return c;
}

void __ws8211_update(uint8_t colors[], uint16_t count) {
	//pixels pointer will be stored in R12
	//count is stored in R13 by default, move to R14

	//Node: P1OUT is hardware register 021h

	//Disable interrupts
	__bic_SR_register(GIE);

	__asm(
			//Initializing stuff
			" PUSH R11;\n"					//Save R11 on the stack
			" PUSH R10;\n"

			//"MOV.W #021h, R10;\n"			//Store P1OUT in R10

			" MOV.W R13, R14;\n"

			" MOV.B #10h, R13;\n"			//Store the pin bit in R13

			" ADD.W R12, R14;\n"			//Keep end pointer in R14
			" INC.W R14;\n"

			" MOV.B @R12+, R15;\n"			//Load current color into R15, increment color pointer (2 cycles)

			" BIT.B #80h, R15;\n"			//Test bit7 (2 cycles)
			" JZ SEND_ZERO_7;\n"			//Jump to label SEND_ZERO_7 if bit7 not set (2 cycles)

			//Sending loop
		"SEND_LOOP:\n"

			//--Bit7--//
			"SEND_ONE_7:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #40h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_6;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_6;\n"

			"SEND_ZERO_7:\n"				//Send 0 routine
				//" nop;\n"					//Delay (1 cycle). This corrects for 1 cycle less spent in SEND_N_0 for this condition
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" BIT.B #40h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_6;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit6--//
			"SEND_ONE_6:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #20h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_5;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_5;\n"		//(2 cycles)

			"SEND_ZERO_6:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #20h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_5;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit5--//
			"SEND_ONE_5:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #10h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_4;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_4;\n"

			"SEND_ZERO_5:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #10h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_4;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit4--//
			"SEND_ONE_4:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #08h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_3;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_3;\n"

			"SEND_ZERO_4:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #08h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_3;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit3--//
			"SEND_ONE_3:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #04h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_2;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_2;\n"

			"SEND_ZERO_3:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #04h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_2;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit2--//
			"SEND_ONE_2:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #02h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_1;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_1;\n"

			"SEND_ZERO_2:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #02h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_1;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			//--Bit1--//
			"SEND_ONE_1:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (12 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BIT.B #10h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_0;\n"		//Jump to label SEND_ZERO_6 (2 cycles)
				" JMP SEND_ONE_0;\n"

			"SEND_ZERO_1:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (4 cycles);
				" nop;\n"
				" nop;\n"
				" nop;\n"

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (8 cycles)
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				" nop;\n"
				//" nop;\n"
				" BIT.B #01h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_0;\n"		//Jump to label SEND_ZERO_6 (2 cycles)

			"SEND_ONE_0:\n"					//Send one routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (3 cycles)
				" nop;\n"
				//" nop;\n"
				" MOV.B @R12+, R15;\n"		//Load current color into R15, increment color pointer (2 cycles)

				" CMP R12, R14;\n"			//Compare current color pointer to color end pointer (1 cycle)
				" JNE LOOP_CONTINUE_1\n"	//Jump to loop continue label (2 cycles)
					" MOV.W #SEND_DONE, R11;\n"	//Move SEND_DONE label into R11 (branch target) (2 cycles)
					" JMP TARGET_DONE;\n"		//Jump to TARGET_DONE label (2 cycles)
				"LOOP_CONTINUE_1:\n"
					" MOV #LOOP_BACK, R11;\n"	//Move LOOP_BACK label into R11 (branch target) (2 cycles)
					" nop;\n"				//Delay (2 cycles)
					" nop;\n"
				"TARGET_DONE:\n"


				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" BR R11;\n"				//Jump to R11 (branch target) (1 cycle)
				"LOOP_BACK:"
					" BIT.B #80h, R15;\n"			//Compare current color pointer to color end pointer (1 cycle)
					" JZ SEND_ZERO_7;\n"			//Jump to SEND_ZERO_7 label (2 cycles)
					" JMP SEND_ONE_7;\n"			//Jump to SEND_ONE_7 label (2 cycles)


			"SEND_ZERO_0:\n"				//Send 0 routine
				" BIS.B R13, &021h;\n"		//Set output pin (2 cycles)
				" nop;\n"					//Delay (2 cycles);
				" nop;\n"
				" MOV.B @R12+, R15;\n"		//Load current color into R15, increment color pointer (2 cycles)

				" BIC.B R13, &021h;\n"		//Clear output pin (2 cycles)
				" nop;\n"					//Delay (3 cycles)
				" nop;\n"
				//" nop;\n"
				" CMP R12, R14;\n"			//Compare current color pointer to color end pointer (1 cycle)
				" JNE LOOP_CONTINUE_0;\n"	//Jump to loop continue label (2 cycles)
				" JMP SEND_DONE\n"			//Branch to SEND_DONE (2 cycles, not counted in timing)

				"LOOP_CONTINUE_0:\n"
				" BIT.B #80h, R15;\n"		//Test next bit (2 cycles)
				" JZ SEND_ZERO_7;\n"		//Jump to SEND_ZERO_7 label (2 cycles)
				" JMP SEND_ONE_7;\n"		//Branch to SEND_ONE_7 label (2 cycle)

			"SEND_DONE:\n"					//Send done label

			" POP R10;\n"
			" POP R11;\n"					//Restore R11 from the stack
	);

	//Enable interrupts
	__bis_SR_register(GIE);

	//Wait >50us
	volatile uint16_t i = 1000;
	for(; i > 0; i--);
}
