#include "ES_Configure.h"
#include "ES_Framework.h"
#include "HAL.h"

pinNum_t pinName;

/*****************************************************************************
// use this to determine which test harness runs
_Bool testWrite = 1;
_Bool testRead = 0;

uint8_t newSensorVal, oldSensorVal;

// main function
int main(void){
    
    // variable declarations
    uint8_t readVal;
    pinNum_t testPin, sensorPin, outputPin;
    
    uint16_t delayTime = 10000;
    uint16_t i; //index used in iterating over delay time
    
    //digitalWrite test harness for testPin
    if (testWrite){
        testPin = 1;
        pinMode(testPin, OUTPUT);
        while (1){
        digitalWrite(testPin,HIGH);
        for (i = 0; i <= delayTime; i++){} //delay
        digitalWrite(testPin,LOW);
        for (i = 0; i <= delayTime; i++){} //delay
        }
    }
    
    //digitalRead test harness for testPin
    if (testRead){
        testPin = 1;
        sensorPin = RB15; outputPin = RB14;
        pinMode(sensorPin,INPUT);
        pinMode(outputPin,OUTPUT);
        pinMode(testPin, INPUT);
        oldSensorVal = digitalRead(sensorPin);
        
        while (1){
            newSensorVal = digitalRead(sensorPin);
            if (newSensorVal < oldSensorVal){ //check for falling edge of touch sensor
            testPin++;
            }
            if (testPin == 20){
                testPin = 0;
            }
            readVal = digitalRead(testPin);
            digitalWrite(outputPin, readVal);
            oldSensorVal = newSensorVal;
            for (i = 0; i <= delayTime; i++){} //delay
        }
    }
}
******************************************************************************/

// pinMode function
void pinMode(uint8_t pin, uint8_t mode)
{
    pinName = pin;
    //set all pins on ports A and B to digital
    ANSELA = 0;
    ANSELB = 0;
    switch (pinName)
    {
        case 1:
            TRISAbits.TRISA0 = mode; 
            break;
        case 2:
            TRISAbits.TRISA1 = mode;
            break;
        case 3:
            TRISAbits.TRISA2 = mode;
            break;
        case 4:
            TRISAbits.TRISA3 = mode;
            break;
        case 5:
            TRISAbits.TRISA4 = mode;
            break;
        case 6:
            TRISBbits.TRISB0 = mode; 
            break;
        case 7:
            TRISBbits.TRISB1 = mode;
            break;
        case 8:
            TRISBbits.TRISB2 = mode;
            break;
        case 9:
            TRISBbits.TRISB3 = mode;
            break;
        case 10:
            TRISBbits.TRISB4 = mode;
            break;
        case 11:
            TRISBbits.TRISB5 = mode; 
            break;
        case 12:
            TRISBbits.TRISB6 = mode;
            break;
        case 13:
            TRISBbits.TRISB7 = mode;
            break;
        case 14:
            TRISBbits.TRISB8 = mode;
            break;
        case 15:
            TRISBbits.TRISB9 = mode;
            break;
        case 16:
            TRISBbits.TRISB10 = mode;
            break;
        case 17:
            TRISBbits.TRISB11 = mode; 
            break;
        case 18:
            TRISBbits.TRISB12 = mode;
            break;
        case 19:
            TRISBbits.TRISB13 = mode;
            break;
        case 20:
            TRISBbits.TRISB14 = mode;
            break;
        case 21:
            TRISBbits.TRISB15 = mode;
            break;   
    }    
}

// digitalWrite function
void digitalWrite(uint8_t pin, uint8_t value){
    pinName = pin;
    switch (pinName)
    {
        case 1:
            LATAbits.LATA0 = value; 
            break;
        case 2:
            LATAbits.LATA1 = value;
            break;
        case 3:
            LATAbits.LATA2 = value;
            break;
        case 4:
            LATAbits.LATA3 = value; 
            break;
        case 5:
            LATAbits.LATA4 = value;
            break;
        case 6:
            LATBbits.LATB0 = value;
            break;
        case 7:
            LATBbits.LATB1 = value; 
            break;
        case 8:
            LATBbits.LATB2 = value;
            break;
        case 9:
            LATBbits.LATB3 = value;
            break;
        case 10:
            LATBbits.LATB4 = value; 
            break;
        case 11:
            LATBbits.LATB5 = value;
            break;
        case 12:
            LATBbits.LATB6 = value;
            break;
        case 13:
            LATBbits.LATB7 = value; 
            break;
        case 14:
            LATBbits.LATB8 = value;
            break;
        case 15:
            LATBbits.LATB9 = value;
            break;
        case 16:
            LATBbits.LATB10 = value; 
            break;
        case 17:
            LATBbits.LATB11 = value;
            break;
        case 18:
            LATBbits.LATB12 = value;
            break;
        case 19:
            LATBbits.LATB13 = value;
            break;
        case 20:
            LATBbits.LATB14 = value; 
            break;
        case 21:
            LATBbits.LATB15 = value;
            break;  
    }
}

// digitalRead function
uint8_t digitalRead(uint8_t pin){
    uint8_t returnVal;
    pinName = pin;
    switch (pinName)
    {
        case 1:
            returnVal = PORTAbits.RA0;
            break;
        case 2:
            returnVal = PORTAbits.RA1;
            break;
        case 3:
            returnVal = PORTAbits.RA2;
            break;
        case 4:
            returnVal = PORTAbits.RA3;
            break;
        case 5:
            returnVal = PORTAbits.RA4;
            break;
        case 6:
            returnVal = PORTBbits.RB0;
            break;
        case 7:
            returnVal = PORTBbits.RB1; 
            break;
        case 8:
            returnVal = PORTBbits.RB2;
            break;
        case 9:
            returnVal = PORTBbits.RB3;
            break;
        case 10:
            returnVal = PORTBbits.RB4; 
            break;
        case 11:
            returnVal = PORTBbits.RB5;
            break;
        case 12:
            returnVal = PORTBbits.RB6;
            break;
        case 13:
            returnVal = PORTBbits.RB7; 
            break;
        case 14:
            returnVal = PORTBbits.RB8;
            break;
        case 15:
            returnVal = PORTBbits.RB9;
            break;
        case 16:
            returnVal = PORTBbits.RB10; 
            break;
        case 17:
            returnVal = PORTBbits.RB11;
            break;
        case 18:
            returnVal = PORTBbits.RB12;
            break;
        case 19:
            returnVal = PORTBbits.RB13;
            break;
        case 20:
            returnVal = PORTBbits.RB14; 
            break;
        case 21:
            returnVal = PORTBbits.RB15;
            break;  
    }
    return returnVal;
}