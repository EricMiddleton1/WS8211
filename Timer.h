/*
 * Timer.h
 *
 *  Created on: Jan 13, 2016
 *      Author: ericm
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <MSP430.h>
#include <stdint.h>

void Timer_init();

uint32_t Timer_getMillis();

void Timer_waitMillis(uint32_t millis);

#endif /* TIMER_H_ */
