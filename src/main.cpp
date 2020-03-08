// Если код собирается в ардуиона ide
//#define ARDUINO_IDE

#ifndef ARDUINO_IDE
#include <Arduino.h>
#endif

// Для отладочной информации
//#define DEBUG 
#ifdef DEBUG
    #define print(x) Serial.print(x)
    #define println(x) Serial.println(x)
#else
    #define print(x)
    #define println(x)
#endif

#include "liquid_crystal_i2c.h"
#include "nrf24l01.h"
#include "encoder.h"
#include "rf24.h"

RF24* radio;
Encoder* encoder;
LiquidCrystal* lcd;

void setup()
{
#ifdef DUBUG
    Serial.begin(9600);
#endif

}

void loop() {
    
}