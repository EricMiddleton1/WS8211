/*
 * Color.c
 *
 *  Created on: Aug 20, 2016
 *      Author: eric
 */

#include "Color.h"

void Color_fromWheel(uint8_t pos, uint8_t* r, uint8_t* g, uint8_t* b) {
	const uint8_t reds[] = { 255, 255, 0, 0, 0, 255 },
		greens[] = { 0, 255, 255, 255, 0, 0 },
		blues[] = { 0, 0, 0, 255, 255, 255 };

	uint16_t widePos = pos;

	int index = (int)(pos * 6 / 255) % 6, next = (index + 1) % 6;
	float mult = 6 * pos / 255.f - index;

	*r = (uint8_t)(reds[index] * (1 - mult) + reds[next] * mult);
	*g = (uint8_t)(greens[index] * (1 - mult) + greens[next] * mult);
	*b = (uint8_t)(blues[index] * (1 - mult) + blues[next] * mult);
}
