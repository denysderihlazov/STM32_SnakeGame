/*
 * randomiser.c
 *
 *  Created on: Aug 15, 2024
 *      Author: denys
 */

#include "randomiser.h"`


static 						uint32_t x = 123456789;
static 						uint32_t y = 362436069;
static 						uint32_t z = 521288629;
static 						uint32_t w = 88675123;

// PRNG to generate eating random
// Traditionally via XOR method
void init_xorshift32(uint32_t seed) {
    x ^= seed;
    y ^= seed << 8;
    z ^= seed << 16;
    w ^= seed << 24;
}

// Generating pseudo random number using Xorshift32
uint32_t xorshift32(void) {
    uint32_t t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

// Returning pseudo random number
uint32_t rand_range(uint32_t max) {
    return xorshift32() % (max + 1);
}
