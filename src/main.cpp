
#include <Arduino.h>
#include <SPI.h>

#include "liquid_crystal_i2c.h"
#include "nrf24l01.h"
#include "encoder.h"
#include "rf24.h"

#define pinCeRf24 0
#define pinCsRf24 0
#define pinEncSw  2
#define pinEncDt  3
#define pinEncClk 4

#define SLAVE  0
#define MASTER 1

#define MODE SLAVE

RF24 radio(pinCeRf24, pinCsRf24);
Encoder encoder(pinEncClk, pinEncDt, pinEncSw);
LiquidCrystal lcd(0x27, 16, 2);

uint8_t attachPCINT(uint8_t pin);

void setup() {
    Serial.begin(9600);
    lcd.begin();

    attachPCINT(pinEncClk);
    attachPCINT(pinEncDt);
}

void loop() {
    
}

uint8_t attachPCINT(uint8_t pin) {
    if (pin < 8) { // D0-D7 // PCINT2
        PCICR |= (1 << PCIE2);
        PCMSK2 |= (1 << pin);
        return 2;
    }
    else if (pin > 13) { //A0-A5  // PCINT1
        PCICR |= (1 << PCIE1);
        PCMSK1 |= (1 << pin - 14);
        return 1;
    }
    else { // D8-D13  // PCINT0
        PCICR |= (1 << PCIE0);
        PCMSK0 |= (1 << pin - 8);
        return 0;
    }
}

ISR(PCINT0_vect) {
    //enc1.tick();
}
ISR(PCINT1_vect) {
    //enc1.tick();
}
ISR(PCINT2_vect) {
    encoder.tick();
}