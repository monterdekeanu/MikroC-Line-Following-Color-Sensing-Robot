/*
 * File:   main.c
 * Author: Keanu Anthony Monterde
 *
 * This is the final
 * Created on May 12, 2024, 3:58 AM
 */


#include <xc.h>

// Configuration bits
#pragma config FOSC = HS // High-speed oscillator
#pragma config WDTE = OFF // Watchdog Timer disabled
#pragma config PWRTE = OFF // Power-up Timer disabled
#pragma config BOREN = ON // Brown-out Reset enabled
#pragma config LVP = OFF // Low Voltage ICSP disabled
#pragma config CPD = OFF // Data EEPROM memory code protection off
#pragma config WRT = OFF // Flash Program Memory write protection off
#pragma config CP = OFF // Code protection off

#define _XTAL_FREQ 20000000 // Define the system frequency
// Include the I2C definitions and initialization functions from the previous context
#include "i2c.h"

#define TCS34725_ADDRESS     0x29  // Default I2C address
#define TCS34725_ENABLE      0x00
#define TCS34725_ATIME       0x01
#define TCS34725_CONTROL     0x0F
#define TCS34725_CDATAL      0x14
#define TCS34725_RDATAL      0x16
#define TCS34725_GDATAL      0x18
#define TCS34725_BDATAL      0x1A
#define TCS34725_COMMAND_BIT 0x80
#define TCS34725_WORD_BIT    0x20

void TCS34725_Init(void);
void TCS34725_ReadColors(uint16_t *red, uint16_t *green, uint16_t *blue);

void ExecuteColorLogic(uint16_t red, uint16_t green, uint16_t blue) {
    PORTD = 0; // Clear PORTD
    if (red > green && red > blue) {//
        PORTD = 0x04; // Red
    } else if (green > red && green > blue) {
        PORTD = 0x08; // Green
    } else if (blue > red && blue > green) {
        PORTD = 0x10; // Blue
    } else {
        PORTD = 0xFF; // White or other color
    }
}


void init(){
    TRISB = 0x00; // PORTB as output (motor control)
    TRISD = 0xFF; // RD0, RD1, RD2 as inputs (IR sensors) 1111
    PORTB = 0x00; // Initialize PORTB to 0

    // Setup PWM using CCP modules for motor speed control
    PR2 = 0xFF; // Timer2 period
    T2CON = 0x07; // Timer2 on, prescale = 1:16

    // PWM mode on CCP1
    CCP1CON = 0x0C; // PWM mode, bits 5-4 = 11 (single output, active high)
    CCPR1L = 0x7F; // Duty cycle (50%)
    TRISC2 = 0; // Set RC2 as output
    
    // PWM mode on CCP2
    CCP2CON = 0x0C;
    CCPR2L = 0x7F;
    TRISC1 = 0; // Set RC1 as output
    
    // Start Timer2
    TMR2ON = 1;
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
    // Convert speed percentage to PWM duty cycle
    CCPR1L = leftSpeed;  // Convert percentage to scale 0-100
    CCPR2L = rightSpeed; // Convert percentage to scale 0-100
}

#define MOTOR_LEFT_FORWARD PORTBbits.RB0
#define MOTOR_LEFT_BACKWARD PORTBbits.RB1
#define MOTOR_RIGHT_FORWARD PORTBbits.RB2
#define MOTOR_RIGHT_BACKWARD PORTBbits.RB3

void turnLeft(){
    MOTOR_LEFT_FORWARD = 0;
    MOTOR_LEFT_BACKWARD = 1;
    MOTOR_RIGHT_FORWARD = 1;
    MOTOR_RIGHT_BACKWARD = 0;
}
void turnRight(){
    MOTOR_LEFT_FORWARD = 1;
    MOTOR_LEFT_BACKWARD = 0;
    MOTOR_RIGHT_FORWARD = 0;
    MOTOR_RIGHT_BACKWARD = 1;
}

void forward(){
    MOTOR_LEFT_FORWARD = 1;
    MOTOR_LEFT_BACKWARD = 0;
    MOTOR_RIGHT_FORWARD = 1;
    MOTOR_RIGHT_BACKWARD = 0;
}
void motorStop(){
    MOTOR_LEFT_FORWARD = 0;
    MOTOR_LEFT_BACKWARD = 0;
    MOTOR_RIGHT_FORWARD = 0;
    MOTOR_RIGHT_BACKWARD = 0;
}
void moveBack(){
    MOTOR_LEFT_FORWARD = 0;
    MOTOR_LEFT_BACKWARD = 1;
    MOTOR_RIGHT_FORWARD = 0;
    MOTOR_RIGHT_BACKWARD = 1;
}


int terminalTracker=0;
void controlMotorsSimplified() {
    char center = PORTDbits.RD1;
    char left = PORTDbits.RD2;
    char right = PORTDbits.RD0;
    if(center == 1 && left == 1 && right == 1){
        setMotorSpeed(45, 45);
        forward();
        terminalTracker++;
        __delay_ms(750);
        return;
    }
    if(terminalTracker > 4){
        motorStop();
        return;
    }
    
    if(left == 1){
        // If left is on, turn left
        setMotorSpeed(100, 90); // Adjust speed as necessary // 100 90
        turnLeft();
        //PORTB = 0x06; // Adjust motor control bits for a left turn
    }else if(right == 1){
        // If right is on, turn right
        setMotorSpeed(100, 90); // Adjust speed as necessary // 100 90
        
        turnRight();
        //PORTB = 0x09; // Adjust motor control bits for a right turn
    }else{
        setMotorSpeed(50, 50);
        forward();
    }
}
void motorStarter(){
    setMotorSpeed(80, 80); // Adjust speed as necessary
    forward();
    __delay_ms(100);
}

int main(){
    
    
    init(); // Initialize everything
    int starter = 0; // Motor speed boost for intertia flag
    
    I2C_Master_Init();
    uint16_t red, green, blue;
    TCS34725_Init();
    green = 255;
    red = 0;
    blue = 0;
    while(1){
        
        if(!PORTDbits.RD3){
            motorStop();
            int buffer = 0;
            while(buffer < 5){
                TCS34725_ReadColors(&red, &green, &blue);
                __delay_ms(30);
                buffer++;
            }
            starter = 0;
        }
        if (red > green && red > blue) {//
            motorStop();
        } else if (green > red && green > blue) {
            if(starter == 0){
                motorStarter();
                starter=1;
            }
            controlMotorsSimplified(); // Check and adjust motors based on sensors
        } else if (blue > red && blue > green) {
            if(starter == 0){
                starter=1;
                setMotorSpeed(80, 80);
                moveBack();
                __delay_ms(100);
            }
            setMotorSpeed(45, 45);
            moveBack(); // Blue
            __delay_ms(1500);
            motorStarter();
            green = 200;
            blue = 0;
            red = 0;
        } else {
            setMotorSpeed(50, 50);
            forward();
            
        }
        
        __delay_ms(20); // Simple debouncing/delay
    }
    return 0;
}


void TCS34725_Init(void) {
    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1);
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_ENABLE);
    I2C_Master_Write(0x03);  // Power on and enable RGBC
    I2C_Master_Stop();

    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1);
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_ATIME);
    I2C_Master_Write(0xD5);  // Set integration time to 101 ms
    I2C_Master_Stop();

    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1);
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_CONTROL);
    I2C_Master_Write(0x00);  // Set gain to 1x
    I2C_Master_Stop();
}

void TCS34725_ReadColors(uint16_t *red, uint16_t *green, uint16_t *blue) {
    unsigned char low, high;

    // Read Red data
    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1); 
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_WORD_BIT | TCS34725_RDATAL);
    I2C_Master_RepeatedStart();
    I2C_Master_Write((TCS34725_ADDRESS << 1) | 1);
    low = I2C_Read_Byte(); 
    I2C_ACK();
    high = I2C_Read_Byte(); 
    I2C_NACK();
    I2C_Master_Stop(); //
    *red = (high << 8) | low; 

    // Read Green data
    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1);
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_WORD_BIT | TCS34725_GDATAL);
    I2C_Master_RepeatedStart();
    I2C_Master_Write((TCS34725_ADDRESS << 1) | 1);
    low = I2C_Read_Byte();
    I2C_ACK();
    high = I2C_Read_Byte();
    I2C_NACK();
    I2C_Master_Stop();
    *green = (high << 8) | low;

    // Read Blue data
    I2C_Master_Start();
    I2C_Master_Write(TCS34725_ADDRESS << 1);
    I2C_Master_Write(TCS34725_COMMAND_BIT | TCS34725_WORD_BIT | TCS34725_BDATAL);
    I2C_Master_RepeatedStart();
    I2C_Master_Write((TCS34725_ADDRESS << 1) | 1);
    low = I2C_Read_Byte();
    I2C_ACK();
    high = I2C_Read_Byte();
    I2C_NACK();
    I2C_Master_Stop();
    *blue = (high << 8) | low;
}
