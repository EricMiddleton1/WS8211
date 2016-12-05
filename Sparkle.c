/*
 * Sparkle.c
 *
 *  Created on: Nov 18, 2016
 *      Author: eric
 */


#include <stdlib.h>
#include "Sparkle.h"
#include "Color.h"

#include "Timer.h"

#define SPARKLE_TIME	1000

static void _makeSparkle(SparkleEffect*, uint8_t);

void Sparkle_init(SparkleEffect *effect, uint8_t sparkleCount, uint8_t ledCount) {
	sparkleCount = (sparkleCount > MAX_SIZE) ? MAX_SIZE : sparkleCount;

	effect->sparkleCount = sparkleCount;
	effect->ledCount = ledCount / 4;

	while(sparkleCount-- > 0) {
		effect->sparkles[sparkleCount].c = Color_rgb(0, 0, 0);
		effect->sparkles[sparkleCount].led = sparkleCount % ledCount;
		effect->sparkles[sparkleCount].onTime = (uint32_t)sparkleCount * SPARKLE_TIME / effect->sparkleCount - SPARKLE_TIME;
	}
}

void Sparkle_update(SparkleEffect *effect, WS8211_t *strip) {
	uint32_t time = Timer_getMillis();

	uint16_t i;
	for(i = 0; i < effect->sparkleCount; ++i) {
		if((effect->sparkles[i].onTime + SPARKLE_TIME) <= time) {
			//Effect has completed

			//Clear LED
			ws8211_setPixel(strip, effect->sparkles[i].led, 0, 0, 0);
			ws8211_setPixel(strip, effect->sparkles[i].led + effect->ledCount, 0, 0, 0);
			ws8211_setPixel(strip, effect->sparkles[i].led + 2*effect->ledCount, 0, 0, 0);
			ws8211_setPixel(strip, effect->sparkles[i].led + 3*effect->ledCount, 0, 0, 0);

			//Randomly generate new sparkle
			_makeSparkle(effect, i);
		}
		else {
			//Linearly ramp brightness
			uint32_t dt = (time - effect->sparkles[i].onTime);
			uint32_t brightness;

			if(dt < (SPARKLE_TIME/2))
				brightness = 2*255*dt / SPARKLE_TIME;
			else
				brightness = 255 - 255*(dt-SPARKLE_TIME/2) / (SPARKLE_TIME/2);

			//Set the color
			ws8211_setPixel(strip, effect->sparkles[i].led,
					brightness * effect->sparkles[i].c.r / 255,
					brightness * effect->sparkles[i].c.g / 255,
					brightness * effect->sparkles[i].c.b / 255);
			ws8211_setPixel(strip, effect->sparkles[i].led + effect->ledCount,
					brightness * effect->sparkles[i].c.g / 255,
					brightness * effect->sparkles[i].c.b / 255,
					brightness * effect->sparkles[i].c.r / 255);
			ws8211_setPixel(strip, effect->sparkles[i].led + 2*effect->ledCount,
					brightness * effect->sparkles[i].c.b / 255,
					brightness * effect->sparkles[i].c.r / 255,
					brightness * effect->sparkles[i].c.g / 255);
			ws8211_setPixel(strip, effect->sparkles[i].led + 3*effect->ledCount,
					brightness * effect->sparkles[i].c.r / 255,
					brightness * effect->sparkles[i].c.b / 255,
					brightness * effect->sparkles[i].c.g / 255);
		}
	}

	//Update the strip
	ws8211_update(strip);
}

void _makeSparkle(SparkleEffect *effect, uint8_t i) {
	Sparkle *sparkle = &(effect->sparkles[i]);
	Color_fromWheel(rand(),
			&(sparkle->c.r),
			&(sparkle->c.g),
			&(sparkle->c.b));

	uint8_t ledPick;
	do {
		ledPick = rand() % effect->ledCount;
	} while(_ledInList(effect, ledPick));

	sparkle->led = ledPick;

	sparkle->onTime = Timer_getMillis();
}

uint8_t _ledInList(SparkleEffect *effect, uint8_t led) {
	uint8_t found = 0;

	uint8_t i;
	for(i = 0; i < effect->sparkleCount; ++i) {
		if(effect->sparkles[i].led == led) {
			found = 1;
			break;
		}
	}

	return found;
}
