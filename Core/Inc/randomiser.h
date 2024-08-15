/*
 * randomiser.h
 *
 *  Created on: Aug 15, 2024
 *      Author: denys
 */

#ifndef SRC_RANDOMISER_H_
#define SRC_RANDOMISER_H_

#include "main.h"


void init_xorshift32(uint32_t seed);

uint32_t xorshift32(void);

uint32_t rand_range(uint32_t max);

#endif /* SRC_RANDOMISER_H_ */
