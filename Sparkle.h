/*
 * Sparkle.h
 *
 *  Created on: Nov 18, 2016
 *      Author: eric
 */

#ifndef SPARKLE_H_
#define SPARKLE_H_

#include <stdint.h>

#include "WS8211.h"

#define MAX_SIZE	10

typedef struct Sparkle {
	uint8_t led;
	int32_t onTime;
	Color c;
} Sparkle;

typedef struct SparkleEffect {
	Sparkle sparkles[MAX_SIZE];
	uint8_t sparkleCount, ledCount;
} SparkleEffect;


void Sparkle_init(SparkleEffect*, uint8_t, uint8_t);

void Sparkle_update(SparkleEffect*, WS8211_t*);


#endif /* SPARKLE_H_ */
