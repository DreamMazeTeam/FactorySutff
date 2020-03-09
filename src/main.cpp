
#include <Arduino.h>
#include <SPI.h>

#include "liquid_crystal_i2c.h"
#include "nrf24l01.h"
#include "encoder.h"
#include "rf24.h"

#define pinCeRf24 0
#define pinCsRf24 0
#define pinEncSw  0
#define pinEncClk 0
#define pinEncDt  0

#define SLAVE  0
#define MASTER 1

#define MODE SLAVE

RF24 radio(pinCeRf24, pinCsRf24);
Encoder encoder(pinEncClk, pinEncDt, pinEncSw);
LiquidCrystal lcd(0x68, 12, 2);

void setup()
{

}

void loop() {
    
}