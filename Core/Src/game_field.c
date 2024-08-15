/*
 * game_field.c
 *
 *  Created on: Aug 15, 2024
 *      Author: denys
 */


#include "game_field.h"
#include "randomiser.h"


static uint8_t grid[WIDTH][HEIGHT];
// Static variables for the snake's segments
static SnakeSegment segment;
static SnakeSegment *snakePreTail;
static SnakeSegment *snakeTail;

// Retrieve the value of a grid cell
uint8_t getGridValue(int x, int y) {
    return grid[x][y];
}

// Set the value of a grid cell
void setGridValue(int x, int y, uint8_t value) {
    grid[x][y] = value;
}


// Initialize the snake game by setting up the snake body
void initSnakeGame(void) {
    initSnakeBody(&segment, &snakePreTail, &snakeTail);
}

// Return the snake's head segment
SnakeSegment* getSnakeSegment(void) {
    return &segment;
}

// Return the snake's pre-tail segment
SnakeSegment* getSnakePreTail(void) {
    return snakePreTail;
}

// Return the snake's tail segment
SnakeSegment* getSnakeTail(void) {
    return snakeTail;
}

// ================= GAME FIELD START =================

// Initialize the snake's body segments and set their initial positions
void initSnakeBody(SnakeSegment *head, SnakeSegment **snakePreTail, SnakeSegment **snakeTail) {
    *snakePreTail = malloc(sizeof(SnakeSegment));
    if (*snakePreTail == NULL) {
        return;
    }

    *snakeTail = malloc(sizeof(SnakeSegment));
    if (*snakeTail == NULL) {

        return;
    }

    head->x = 10;
    head->y = 15;
    head->prev = *snakePreTail;

    (*snakePreTail)->x = 10;
    (*snakePreTail)->y = 16;
    (*snakePreTail)->prev = *snakeTail;

    (*snakeTail)->x = 10;
    (*snakeTail)->y = 17;
    (*snakeTail)->prev = NULL;
}

// Add a new tail segment when the snake eats food
void addTaleWhenFood(SnakeSegment **snakeTail, int x, int y, SnakeSegment **snakePreTail) {
    if (*snakeTail == NULL) {
        return;
    }

    SnakeSegment *newTail = malloc(sizeof(SnakeSegment));
    newTail->x = x;
    newTail->y = y;
    newTail->prev = NULL;

    (*snakeTail)->prev = newTail;

    // Update the snake's pre-tail and tail pointers
    // Make Tail as PreTail
    *snakePreTail = (*snakePreTail)->prev;
    *snakeTail = newTail;
}

// Initialize the game field by setting all tiles as empty and drawing the initial snake position
void initGameField(void) {
    // Set all game field tiles as EMPTY
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            grid[x][y] = EMPTY;
        }
    }

    // Snake start position
    grid[10][15] = SNAKE;
    initSnakeBody(&segment, &snakePreTail, &snakeTail);
    Paint_DrawRectangle(10*CELL_SIZE, 15*CELL_SIZE, (10*CELL_SIZE)+CELL_SIZE, (15*CELL_SIZE)+CELL_SIZE, BLACK, 1, 1);
    // Snake pretail position
    grid[10][16] = SNAKE;
    Paint_DrawRectangle(10*CELL_SIZE, 16*CELL_SIZE, (10*CELL_SIZE)+CELL_SIZE, (16*CELL_SIZE)+CELL_SIZE, BLACK, 1, 1);
    // Snake pretail position
    grid[10][17] = SNAKE;
    Paint_DrawRectangle(10*CELL_SIZE, 17*CELL_SIZE, (10*CELL_SIZE)+CELL_SIZE, (17*CELL_SIZE)+CELL_SIZE, BLACK, 1, 1);
}
// ================= GAME FIELD END =================


// ================= EATING RANDOMISER START =================

// Randomly place food on the game field in an empty cell
void eatRandomizer(uint8_t *eatingFieldCount) {
	int emptyCellCount = 0;
	int emptyCells[WIDTH*HEIGHT];

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
	        if (grid[x][y] == EMPTY) {
	            emptyCells[emptyCellCount++] = y * WIDTH + x;
	        }
		}
	}

	// Randomly place food in one of the empty cells
	if(emptyCellCount>0) {
		uint32_t randomIndex = rand_range(emptyCellCount);
		int cell = emptyCells[randomIndex];
		grid[cell % WIDTH][cell/WIDTH] = FOOD;
		Paint_DrawRectangle((cell % WIDTH)*CELL_SIZE,
			(cell/WIDTH)*CELL_SIZE,
			((cell % WIDTH)*CELL_SIZE)+CELL_SIZE,
			((cell/WIDTH)*CELL_SIZE)+CELL_SIZE,
			EAT,
			1,
			1
		);
	}

	(*eatingFieldCount)++;
}
// ================= GAME FIELD END =================
