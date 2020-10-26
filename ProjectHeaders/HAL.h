#ifndef HAL
#define HAL

// Function Prototypes
void pinMode(uint8_t pin, uint8_t mode); //set pin as either input or output
void digitalWrite(uint8_t pin, uint8_t value); //write to pin
uint8_t digitalRead(uint8_t pin); //read from pin

// map pin names to pin numbers 
typedef enum
{
    RA0 = 1, RA1, RA2, RA3, RA4, RB0, RB1, RB2, RB3, RB4,
    RB5, RB6, RB7, RB8, RB9, RB10, RB11, RB12, RB13,
    RB14, RB15
} pinNum_t;

// definitions
#define OUTPUT 0
#define INPUT 1
#define HIGH 1
#define LOW 0

#endif 