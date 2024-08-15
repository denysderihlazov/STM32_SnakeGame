/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"

#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_Test.h"
#include "DEV_Config.h"
#include "LCD_2inch4.h"

// All MACROS are here in config.h
#include "config.h"

#include "randomiser.h"
#include "game_field.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// ======== Controlling ========
uint16_t 					joystickX = 0;
uint16_t 					joystickY = 0;
char 						joystickXStr[16];
char 						joystickYStr[16];

volatile uint16_t			adcResultDMA [3];
const int 					adcChannelCount = sizeof (adcResultDMA) / sizeof(adcResultDMA[0]);
volatile int				adcConversionComplete = 0;

char 						normalisedXStr[16];
char 						normalisedYStr[16];

int 						normalisedX = 0;
int 						normalisedY = 0;

uint8_t eatingFieldCount = 0;

static uint32_t 			lastDirectionChangeTime = 0;
static uint32_t 			lastSnakeMovementTime = 0;
static uint32_t 			lastFoodAppearanceTime = 0;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} SnakeDirection;
SnakeDirection currentSnakeDirection = UP; // Initially UP

uint16_t					snakeLength = 0;

uint32_t 					currentTime = 0; // To compare system ticks

uint8_t						GameOver = 0;
uint16_t					score = 0; // Meals eaten

uint16_t 					tempRawValue = 0;
uint32_t 					randomizedValue; // Last digit of raw Temp * joystickX + joystickY

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void drawSnakeEyes(int snakeHeadX, int snakeHeadY);

/* These function prototypes correspond to the functions implemented in the USER CODE 4 section */
void snakeSelfMovement(SnakeSegment *SnakeHead, SnakeSegment **snakePreTail, SnakeSegment **snakeTail, SnakeDirection currentSnakeDirection, uint16_t *score, uint8_t *eatingFieldCount);
void drawSnakeEyes(int snakeHeadX, int snakeHeadY);
int rawDataToNormalised(int raw, int min, int max, int center);
void moveHead(int currentHeadX, int currentHeadY);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void initLCD()
{
	DEV_Module_Init();

	LCD_2IN4_Init();

	Paint_NewImage(LCD_2IN4_WIDTH,LCD_2IN4_HEIGHT, ROTATE_180, WHITE);

	Paint_SetClearFuntion(LCD_2IN4_Clear);
	Paint_SetDisplayFuntion(LCD_2IN4_DrawPaint);
	Paint_Clear(BG);
}

// Waiting till the end of conversion to change flag
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	adcConversionComplete = 1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  initLCD();

  initGameField();


  initSnakeGame();
  SnakeSegment* segment = getSnakeSegment();
  SnakeSegment* snakePreTail = getSnakePreTail();
  SnakeSegment* snakeTail = getSnakeTail();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcResultDMA, adcChannelCount);
      while (adcConversionComplete == 0) {
          // Waiting till the end of ADC conversion
      }
      adcConversionComplete = 0;

      // Convert raw data from thumbstick
      int normalisedX = rawDataToNormalised(adcResultDMA[0], 370, 1023, 918);
      int normalisedY = rawDataToNormalised(adcResultDMA[1], 268, 1023, 755);

      // Thumbstick debug info output on LCD
      //Paint_DrawNum(30, 240, adcResultDMA[0], &Font24, BG, WHITE);
      //Paint_DrawNum(30, 270, adcResultDMA[1], &Font24, BG, WHITE);

      currentTime = HAL_GetTick();

      if ((currentTime - lastSnakeMovementTime) > SNAKE_SPEED  && GameOver != 1000000) {
          //snakeSelfMovement(&segment, &snakePreTail, &snakeTail, currentSnakeDirection, &score, &eatingFieldCount);
          snakeSelfMovement(segment, &snakePreTail, &snakeTail, currentSnakeDirection, &score, &eatingFieldCount);

          lastSnakeMovementTime = currentTime;
      }

      // Snake logic for controlling the movement direction
      if ((currentTime - lastDirectionChangeTime) > CHANGE_DIRECTION_DELAY) {
          moveHead(normalisedX, normalisedY);
          lastDirectionChangeTime = currentTime;
      }

      // Check if enough time has passed since the last food appearance and if there are less than 2 food items on the field.
      // If true, generate a randomized value using ADC inputs, initialize the random number generator with this value,
      // and place a new food item on the game field. Update the timestamp for the last food appearance.
      if ((currentTime - lastFoodAppearanceTime) > EATING_APPEARANCE_PERIOD && eatingFieldCount < 2) {
          randomizedValue = (adcResultDMA[2] % 10 * adcResultDMA[0]) + adcResultDMA[1];
          init_xorshift32(randomizedValue);
          eatRandomizer(&eatingFieldCount);
          lastFoodAppearanceTime = currentTime;
      }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ================= SNAKE SELF MOVEMENT START =================
void snakeSelfMovement(SnakeSegment *SnakeHead, SnakeSegment **snakePreTail, SnakeSegment **snakeTail, SnakeDirection currentSnakeDirection, uint16_t *score, uint8_t *eatingFieldCount) {
	int isEat = 0; // If next tile has EAT

    int nextX = SnakeHead->x;
    int nextY = SnakeHead->y;
    int oldX = SnakeHead->x;
    int oldY = SnakeHead->y;

    int oldTailX = (*snakeTail)->x;
    int oldTailY = (*snakeTail)->y;

	// Move snake head
    if (currentSnakeDirection == UP) {
        nextY -= 1;
    } else if (currentSnakeDirection == DOWN) {
        nextY += 1;
    } else if (currentSnakeDirection == LEFT) {
        nextX -= 1;
    } else if (currentSnakeDirection == RIGHT) {
        nextX += 1;
    }

    // Check if next step isn't out side of game field
    if (nextX < 0 || nextX >= WIDTH || nextY < 0 || nextY >= HEIGHT) {
        Paint_DrawString_EN(45, 150, "GAME OVER", &Font24, BLACK, WHITE);
        char scoreText[50]; // Score output
        sprintf(scoreText, "Score: %d", *score); // Format Score string
        Paint_DrawString_EN(45, 174, scoreText, &Font24, BLACK, WHITE);

        GameOver += 1;
        return;
    }

    // Check if next tale has FOOD
    if (getGridValue(nextX, nextY) == FOOD) {
        isEat = 1;
    }

    SnakeHead->x = nextX;
    SnakeHead->y = nextY;
    setGridValue(nextX, nextY, SNAKE);

    // Draw new head position
    Paint_DrawRectangle(SnakeHead->x * CELL_SIZE, SnakeHead->y * CELL_SIZE, (SnakeHead->x * CELL_SIZE) + CELL_SIZE, (SnakeHead->y * CELL_SIZE) + CELL_SIZE, PLAYER, 1, 1);
    drawSnakeEyes(SnakeHead->x, SnakeHead->y);

    // Move rest of snake segments
    SnakeSegment *currentSegment = SnakeHead->prev;
    while (currentSegment != NULL) {
        int currentX = currentSegment->x;
        int currentY = currentSegment->y;

        currentSegment->x = oldX;
        currentSegment->y = oldY;

        Paint_DrawRectangle(currentSegment->x * CELL_SIZE, currentSegment->y * CELL_SIZE, (currentSegment->x * CELL_SIZE) + CELL_SIZE, (currentSegment->y * CELL_SIZE) + CELL_SIZE, PLAYER, 1, 1);

        // Save coordinates for the next segment
        oldX = currentX;
        oldY = currentY;

        if (currentSegment->prev != NULL && currentSegment->prev->prev == NULL) {
            *snakePreTail = currentSegment;
        }
        currentSegment = currentSegment->prev;
    }

    if(!isEat) {
        // if isEat = false then clean last tail pos
        Paint_DrawRectangle(oldTailX * CELL_SIZE, oldTailY * CELL_SIZE, (oldTailX * CELL_SIZE) + CELL_SIZE, (oldTailY * CELL_SIZE) + CELL_SIZE, BG, 1, 1);

        // clean last tail pos on a grid
        setGridValue(oldTailX, oldTailY, EMPTY);

        *snakeTail = (*snakePreTail)->prev;
    } else {
    	addTaleWhenFood(snakeTail, oldTailX, oldTailY, snakePreTail);

        // if isEat = true -> tale+1
        Paint_DrawRectangle(oldTailX * CELL_SIZE, oldTailY * CELL_SIZE, (oldTailX * CELL_SIZE) + CELL_SIZE, (oldTailY * CELL_SIZE) + CELL_SIZE, PLAYER, 1, 1);
        // clean last tail pos on a grid
        setGridValue(oldTailX, oldTailY, SNAKE);
        (*score)++;

        isEat = 0;
        (*eatingFieldCount)--;
    }
}

void drawSnakeEyes(int snakeHeadX, int snakeHeadY) {
	if(currentSnakeDirection == UP) {
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 1, snakeHeadY * CELL_SIZE + 2, (snakeHeadX * CELL_SIZE) + 2, (snakeHeadY * CELL_SIZE) + 4, WHITE, 1, 1);
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 8, snakeHeadY * CELL_SIZE + 2, (snakeHeadX * CELL_SIZE) + 9, (snakeHeadY * CELL_SIZE) + 4, WHITE, 1, 1);
	} else if (currentSnakeDirection == LEFT) {
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 1, snakeHeadY * CELL_SIZE + 1, (snakeHeadX * CELL_SIZE) + 2, (snakeHeadY * CELL_SIZE) + 3, WHITE, 1, 1);
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 1, snakeHeadY * CELL_SIZE + 7, (snakeHeadX * CELL_SIZE) + 2, (snakeHeadY * CELL_SIZE) + 9, WHITE, 1, 1);
	} else if (currentSnakeDirection == DOWN) {
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 1, snakeHeadY * CELL_SIZE + 6, (snakeHeadX * CELL_SIZE) + 2, (snakeHeadY * CELL_SIZE) + 8, WHITE, 1, 1);
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 8, snakeHeadY * CELL_SIZE + 6, (snakeHeadX * CELL_SIZE) + 9, (snakeHeadY * CELL_SIZE) + 8, WHITE, 1, 1);
	} else if (currentSnakeDirection == RIGHT) {
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 8, snakeHeadY * CELL_SIZE + 1, (snakeHeadX * CELL_SIZE) + 9, (snakeHeadY * CELL_SIZE) + 3, WHITE, 1, 1);
		Paint_DrawRectangle(snakeHeadX * CELL_SIZE + 8, snakeHeadY * CELL_SIZE + 7, (snakeHeadX * CELL_SIZE) + 9, (snakeHeadY * CELL_SIZE) + 9, WHITE, 1, 1);
	}
}
// ================= SNAKE SELF MOVEMENT END =================


// ================= CONTROLS START =================
// Convert ADC thumbnail raw data into normal
int rawDataToNormalised(int raw, int min, int max, int center) {
    int range = max - min;
    int normalised = ((raw - center) * 200) / range;
    return normalised;
}


void moveHead(int currentHeadX, int currentHeadY) {
    if (HAL_GetTick() - lastDirectionChangeTime < CHANGE_DIRECTION_DELAY) {
        return;
    }

    SnakeDirection newDirection = currentSnakeDirection;

    // Direction priority
    if (abs(currentHeadX) > abs(currentHeadY)) { // Horizontal priority
        if (currentHeadX > DEAD_ZONE && currentSnakeDirection != LEFT) {
            newDirection = RIGHT;
        } else if (currentHeadX < -DEAD_ZONE && currentSnakeDirection != RIGHT) {
            newDirection = LEFT;
        }
    } else { // Vertical priority
        if (currentHeadY > DEAD_ZONE && currentSnakeDirection != DOWN) {
            newDirection = UP;
        } else if (currentHeadY < -DEAD_ZONE && currentSnakeDirection != UP) {
            newDirection = DOWN;
        }
    }

    if (newDirection != currentSnakeDirection) {
        currentSnakeDirection = newDirection;
        lastDirectionChangeTime = HAL_GetTick();
    }
}
// ================= CONTROLS END =================
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
