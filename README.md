# Line Following & Color Sensing Robot

This project is a line-following and color-sensing robot programmed in mikroC and compiled with MPLAB. The robot uses a TCS34725 color sensor, a PIC16F877A microcontroller, and an L298N motor driver. It is designed to stop on red, go on green, and reverse on blue. The robot also utilizes 3-way IR sensors for line following and an additional IR sensor to switch the color sensor on and off.

## Features

- **Line Following**: Uses 3-way IR sensors to follow a line.
- **Color Sensing**: TCS34725 sensor to detect red, green, and blue colors.
- **Motor Control**: L298N motor driver to control robot movements.
- **Behavior Based on Color**:
  - Stops on red.
  - Moves forward on green.
  - Moves backward on blue.
- **Terminal Detection**: The robot will stop on the 5th terminal, which is the 5th time the 3-way IR sensors detect everything as black.

## Hardware Components

- PIC16F877A Microcontroller
- TCS34725 Color Sensor
- L298N Motor Driver
- 3-way IR Sensors (for line following)
- Single IR Sensor (for switching the color sensor on/off)
- Motors and Chassis

## Software

- **mikroC**: Programming language used.
- **MPLAB**: Compiler used.

## Installation and Setup

1. **Clone the Repository**:
    ```sh
    git clone <repository_url>
    ```

2. **Open MPLAB**:
    - Load the project files into MPLAB.

3. **Compile the Code**:
    - Compile the provided source code using MPLAB.

4. **Flash the Microcontroller**:
    - Flash the compiled code to the PIC16F877A microcontroller using an appropriate programmer.

## Files

- **main.c**: Contains the main program logic.
- **i2c.c**: Contains the I2C communication functions.
- **i2c.h**: Header file for I2C communication functions.

## Usage

1. **Power the Robot**:
    - Ensure that all connections are secure and power up the robot.

2. **Line Following Mode**:
    - The robot will follow the line using the 3-way IR sensors.

3. **Color Sensing Mode**:
    - The robot will stop when it detects red, move forward when it detects green, and move backward when it detects blue.

4. **Terminal Detection**:
    - The robot will stop on the 5th terminal, which is the 5th time the 3-way IR sensors detect everything as black.

## License

This project is licensed under the GNU General Public License v2.0. See the [LICENSE](./LICENSE) file for details.

## Author

- **Keanu Anthony Monterde**
- monterdekeanu@gmail.com
## Acknowledgments

- Special thanks to the developers of mikroC and MPLAB for their tools.
- Thanks to the open-source community for their invaluable resources and support.
- Grateful acknowledgment to my partner, Agnieszka Magos, for her neverending support.
- Many thanks to my classmates for their collaboration with the hardware.
