#include <Arduino.h>
#include <Wire.h>

#define SLAVE   0
#define MASTER  1


#define MODE SLAVE
#define __DEBUG

#if MODE == MASTER
    #define src master
#elif MODE == SLAVE
    #define src slave
#endif


#include "encoder.h"
#include "SerialFlow.h"
#include "liquid_crystal_i2c.h"


#define CE      5
#define CS      6
#define SW      2
#define DT      3
#define CLK     4
#define BUTTON  7
#define SCRADDR 0x27
#define DR      9
#define DL      10
#define MICRO   A0


#define VALUE_LEN 4
#define MICRO_OFFSET 255
#define MICRO_FREQ 4


#if MODE == SLAVE

namespace slave
{
    SerialFlow radio(CE, CS);
    LiquidCrystal lcd(SCRADDR, 16, 2);
    uint32_t buffer;

    void setup();
    void loop();

    void updateScreen();
}

#elif MODE == MASTER

namespace master
{
    Encoder encoder(CLK, DT, SW);
    SerialFlow radio(CE, CS);
    LiquidCrystal lcd(SCRADDR, 16, 2);

    int btnPrevState;
    int btnCurrentState;

    size_t clickDelta = 300;
    size_t lastClick = 0;

    int value[VALUE_LEN] = {0, 0, 0, 0};
    int pos = 0;

    void setup();
    void loop();

    uint8_t attachPCINT(uint8_t pin);
    void updateScreen();
};

#endif

void setup() {

    TCCR1A = TCCR1A & 0xe0 | 3; 
    TCCR1B = TCCR1B & 0xe0 | 0x09;

    src::setup();
}

void loop() {
    src::loop();
}

#if MODE == MASTER

uint8_t master::attachPCINT(uint8_t pin)
{
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

void master::updateScreen()
{
    lcd.clear();
    lcd.setCursor(0, 0);

    for (int i = 0; i < VALUE_LEN; i++) {
        lcd.print(value[i]);
    }

    lcd.setCursor(0, 1);
    lcd.setCursor(pos, 0);
}

void master::setup() 
{
#ifdef __DEBUG
    Serial.begin(9600); 
#endif
    
    pinMode(SW, INPUT);
    pinMode(BUTTON, INPUT);
    pinMode(MICRO, INPUT);

    attachPCINT(CLK);
    attachPCINT(DT);

    lcd.begin();
    lcd.backlight();
    lcd.blink_on();

    encoder.setType(TYPE2);
    encoder.setDirection(REVERSE);

    radio.getPacketValue(1);
    radio.setPacketFormat(2, 2);
    radio.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);

    updateScreen();
}

void master::loop()
{
    btnCurrentState = digitalRead(BUTTON);

    if (encoder.isLeft()) {
        if (value[pos] > 0) value[pos]--;
        else value[pos] = 9;
        updateScreen();
    }
    else if (encoder.isRight()) {
        if (value[pos] < 9) value[pos]++;
        else value[pos] = 0;
        updateScreen();
    }
    else if ((btnCurrentState == LOW) && (btnPrevState == HIGH)) {
        if (pos < 3) pos++;
        else pos = 0;
        updateScreen();
    } 

    #ifdef SMTH
    #endif
    
    uint32_t buffer = value[0] * 1000 + 
                      value[1] *  100 +
                      value[2] *   10 +
                      value[3];

    int mbuffer = analogRead(MICRO);

    radio.setPacketValue((uint32_t)buffer); 
    
    radio.setPacketValue(mbuffer);

    radio.sendPacket();

    btnPrevState = btnCurrentState;

#ifdef __DEBUG

    Serial.println("Sent data");
    Serial.println(buffer);
    Serial.println(mbuffer);
    
#endif
}

#elif MODE == SLAVE

void slave::setup()
{
#ifdef __DEBUG
    Serial.begin(9600);
#endif

    lcd.begin();
    lcd.backlight();
    lcd.blink_off();

    radio.setPacketFormat(2, 1);
    radio.begin(0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL);

    pinMode(DR, OUTPUT); 
    pinMode(DL, OUTPUT);
}

void slave::loop()
{
    if (radio.receivePacket()) {
        // uint32_t volume = radio.getPacketValue(0);
        uint32_t _b = radio.getPacketValue(1);
        int& volume = *(int*)&_b; 

        /* if (data <= 9999 && data >= 0){
            buffer = data;
            updateScreen();

#ifdef __DEBUG
            Serial.print("Digit: ");
            Serial.println(buffer);
#endif
        } */

        if (volume >= 0 && volume <= 1023){
            analogWrite(9, volume);
            analogWrite(10, volume);

#ifdef __DEBUG
            Serial.print("Volume: ");
            Serial.println(volume);
#endif
        }
    }
}

void slave::updateScreen()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buffer);
}

#endif

#if MODE == MASTER

ISR(PCINT0_vect) {
    //enc1.tick();
}
ISR(PCINT1_vect) {
    //enc1.tick();
}
ISR(PCINT2_vect) {
    master::encoder.tick();
}

#endif