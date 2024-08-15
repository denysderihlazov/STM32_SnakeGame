/*
 * config.h
 *
 *  Created on: Aug 15, 2024
 *      Author: denys
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "main.h"

// Game settings
#define 		DEAD_ZONE 30
#define 		MAX_SNAKE_LENGHT 768 // (240/10) * (320/10)
#define 		SNAKE_SPEED 320 // Passive movement once per ms
#define 		EATING_APPEARANCE_PERIOD 2500 // Once per ms
#define			CHANGE_DIRECTION_DELAY 150 // Delay to prevent accidental JoyStick multi-directional input


// Field states
#define 		CELL_SIZE 10 // Each tile is 10x10 pixels
#define 		WIDTH 24
#define 		HEIGHT 32
#define 		EMPTY 0
#define 		SNAKE 1
#define 		FOOD 2

#endif /* INC_CONFIG_H_ */
