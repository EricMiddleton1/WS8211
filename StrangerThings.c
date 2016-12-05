/*
 * StrangerThings.c
 *
 *  Created on: Nov 18, 2016
 *      Author: eric
 */


#include "StrangerThings.h"

#include <string.h>
#include <stdint.h>

static int16_t _getLight(char);

void StrangerThings_display(WS8211_t *lights, const char *str, unsigned int pos, uint8_t brightness) {
	if(pos >= strlen(str)) {
		return;
	}

	//Clear the strip
	ws8211_clear(lights);

	int16_t light = _getLight(str[pos]);

	if(light < 0) {
		//Skip it
	}
	else {
		ws8211_setPixel(lights, light, brightness, brightness, brightness);
	}

	//Update the strip
	ws8211_update(lights);
}

static int16_t _getLight(char c) {
	//Convert to uppercase
	if(c >= 'a' && c <= 'z') {
		c += 'A' - 'a';
	}
	else if(c < 'A' || c > 'Z') {
		//Invalid character
		return -1;
	}

	//In order Z-A starting from 1
	return 26 - (c - 'A');
}
