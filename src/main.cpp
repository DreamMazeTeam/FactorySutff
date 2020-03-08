
#include <Arduino.h>
#include <SPI.h>

#pragma region DEBUG

#ifdef DEBUG

#define print(x) Serial.print(x)
#define println(x) Serial.println(x)

#else

#define print(x)
#define println(x)

#endif

#pragma endregion
#pragma region INCLUDES

#include "liquid_crystal_i2c.h"
#include "nrf24l01.h"
#include "encoder.h"
#include "rf24.h"

#pragma endregion
#pragma region PINPUT

#define pinCeRf24 0
#define pinCsRf24 0
#define pinEncSw  0
#define pinEncClk 0
#define pinEncDt  0

#pragma endregion

RF24* radio;
Encoder* encoder;
LiquidCrystal* lcd;

void setup()
{
#ifdef DUBUG
    Serial.begin(9600);
#endif

    radio = new RF24(pinCeRf24, pinCsRf24);
    encoder = new Encoder(pinEncClk, pinEncDt, pinEncSw);
    lcd = new LiquidCrystal(0x68, 12, 2);
}

void loop() {
    
}