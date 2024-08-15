# STM32 Snake Game 🐍

This repository contains the code for a classic Snake game implemented on the STM32F446RE Nucleo platform. This project is my first major venture into embedded systems, focusing on code optimization and resource management to bring this nostalgic game to life on a microcontroller.

![Snake Game](https://i.imgur.com/TXZyPhQ.jpg)
![Snake Game](https://i.imgur.com/1j07OSV.png)

## Key Features

- **Code Optimization**: The game is optimized to run smoothly on the limited resources of the STM32 microcontroller. This involved careful management of memory and processing power to ensure responsive gameplay.

- **Random Number Generation**: Without a hardware RNG on the STM32F446RE, a creative approach was used to generate random numbers. The code leverages raw 32-bit values from the MCU’s internal thermometer and the joystick's X and Y axes as a Pseudo Random Number Generator (PRNG). This mechanism generates two apples on the 768-cell grid every 2.5 seconds, adding a dynamic element to the game.

## Hardware Used

- **MCU**: STM32F446RE Nucleo
- **LCD**: Waveshare 18366 (240×320, 2.4 inch)
- **Joystick**: SparkFun COM-09426

## Project Structure

The project is divided into several modules, each handling different aspects of the game:

- **main.c**: Contains the main game loop and handles the initialization of peripherals and the game field.
- **game_field.c**: Manages the game field, including the snake's position and the appearance of food.
- **randomiser.c**: Implements the PRNG used to generate random numbers for placing food on the grid.

## Getting Started

To get started with this project:

1. Clone the repository.
git clone https://github.com/denysderihlazov/STM32_SnakeGame.git
2. Open the project in your preferred STM32 IDE (e.g., STM32CubeIDE).
3. Compile and upload the code to your STM32F446RE Nucleo board.
4. Connect the Waveshare LCD and joystick as per the provided pinout configuration.
5. Power on the system and enjoy the game!

## How It Works

- **Snake Movement**: The snake's movement is controlled by the joystick. The direction changes based on the thumbstick's position relative to a dead zone, ensuring responsive and precise control.
- **Food Generation**: Food is randomly placed on the grid every 2.5 seconds, using a custom PRNG algorithm to ensure randomness despite the lack of a hardware RNG on the STM32F446RE.
- **Game Over**: The game ends when the snake runs into the wall or itself, with the final score displayed on the LCD.

## License

This project is licensed under the terms specified in the LICENSE file found in the root directory of this repository.

## Contact

For any questions or feedback, feel free to reach out to me via [LinkedIn](https://www.linkedin.com/in/denysderihlazov/).

---
