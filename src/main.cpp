
#include <Arduino.h>
#include <SPI.h>

#include "liquid_crystal_i2c.h"
#include "SerialFlow.h"
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

#define MODE MASTER

RF24 radio(pinCeRf24, pinCsRf24);
Encoder encoder(pinEncClk, pinEncDt, pinEncSw);
LiquidCrystal lcd(0x27, 16, 2);

uint8_t attachPCINT(uint8_t pin);

SerialFlow rd(5, 6);
const unsigned long data_to = 100; 
unsigned long tm, data_next;

void setup() {
    Serial.begin(9600);
    lcd.begin();

    attachPCINT(pinEncClk);
    attachPCINT(pinEncDt);

    rd.setPacketFormat(2, 1);

#if MODE == SLAVE
    rd.begin(0xF0F0F0F0D2LL,0xF0F0F0F0E1LL);
#elif MODE == MASTER
    rd.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);
#endif
}

void loop() {
#if MODE == SLAVE
    unsigned int v;
    if( rd.receivePacket() ){
        v = rd.getPacketValue(0);
        Serial.println(v);
    }
#elif MODE == MASTER
    tm = millis();
    if( tm > data_next ){
        data_next = tm + data_to;
        rd.setPacketValue( tm );
        rd.sendPacket();
    }
#endif
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