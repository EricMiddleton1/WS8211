/*
 * WS8211.h
 *
 *  Created on: Aug 20, 2016
 *      Author: eric
 */

#ifndef WS8211_H_
#define WS8211_H_

#include "stdint.h"

typedef struct {
	uint8_t *colors;
	uint16_t size;
} WS8211_t;

void ws8211_init(WS8211_t* strip, uint8_t colors[], uint16_t size);

void ws8211_setPixel(WS8211_t* strip, uint16_t id, uint8_t r, uint8_t g, uint8_t b);

void ws8211_getPixel(WS8211_t* strip, uint16_t id, uint8_t* r, uint8_t* g, uint8_t* b);

void ws8211_shiftForward(WS8211_t* strip);

void ws8211_update(WS8211_t* strip);




#endif /* WS8211_H_ */
