/*
 * game_field.h
 *
 *  Created on: Aug 15, 2024
 *      Author: denys
 */

#ifndef INC_GAME_FIELD_H_
#define INC_GAME_FIELD_H_

#include "main.h"

// All MACROS are here in config.h
#include "config.h"

#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_Test.h"
#include "DEV_Config.h"
#include "LCD_2inch4.h"


typedef struct SnakeSegment {
    uint16_t x;
    uint16_t y;
    struct SnakeSegment *prev;
} SnakeSegment;

void initSnakeGame(void);
SnakeSegment* getSnakeSegment(void);
SnakeSegment* getSnakePreTail(void);
SnakeSegment* getSnakeTail(void);

void initSnakeBody(SnakeSegment *head, SnakeSegment **snakePreTail, SnakeSegment **snakeTail);
void addTaleWhenFood(SnakeSegment **snakeTail, int x, int y, SnakeSegment **snakePreTail);
void initGameField();

void eatRandomizer(uint8_t *eatingFieldCount);


uint8_t getGridValue(int x, int y);
void setGridValue(int x, int y, uint8_t value);


#endif /* INC_GAME_FIELD_H_ */
