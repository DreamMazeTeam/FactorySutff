#include <Arduino.h>
#include <Wire.h>

#define SLAVE   0
#define MASTER  1


#define MODE MASTER


#if MODE == MASTER
    #define src master
#elif MODE == SLAVE
    #define src slave
#endif


#include "rf24v.h"
#include "encoder.h"
#include "pcf8591p.h"
#include "SerialFlow.h"
#include "liquid_crystal_i2c.h"


#define CE      5
#define CS      6
#define SW      2
#define DT      3
#define CLK     4
#define BUTTON  7
#define SCRADDR 0x27
#define DIOD 9
#define MICRO A0


#define VALUE_LEN 4


#if MODE == SLAVE

namespace slave
{
    SerialFlow radio(CE, CS);
    SerialFlow radio2(CE, CS);
    LiquidCrystal lcd(SCRADDR, 16, 2);
    RF24V sound(radio.getRf24(), 0);
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
    SerialFlow radio2(CE, CS);
    LiquidCrystal lcd(SCRADDR, 16, 2);
    RF24V sound(radio.getRf24(), 0);
    PCF8591 acp(0x48);

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

#define TEST

#ifdef TEST

SerialFlow rd(CE, CS);
SerialFlow rd2(CE, CS);

    #if MODE == MASTER

    #elif MODE == SLAVE

    #endif
#endif

void setup() {
    // src::setup();
#ifdef TEST

    rd.setPacketFormat(2, 1);
    rd2.setPacketFormat(2, 1);

    #if MODE == MASTER

    rd.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);
    rd2.begin(0xF0F0F0F0E0LL,0xF0F0F0F0D1LL);

    #elif MODE == SLAVE
    
    rd.begin(0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL);
    rd2.begin(0xF0F0F0F0D1LL, 0xF0F0F0F0E0LL);
    Serial.begin(9600);
    
    #endif
#endif
}

void loop() {
    // src::loop();
#ifdef TEST

    int data;

    #if MODE == MASTER

    data = analogRead(MICRO);
    rd.setPacketValue( 228 );
    rd.sendPacket();

    rd2.setPacketValue( 1337 );
    rd2.sendPacket();
    

    #elif MODE == SLAVE

    unsigned int v;
    if( rd.receivePacket() ){
        v = rd.getPacketValue(0);
        Serial.println(v);
    }

    if( rd2.receivePacket() ){
        v = rd2.getPacketValue(0);
        Serial.println(v);
    }
    

    #endif
#endif
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
#ifdef PCF8591_DEBUG
    Serial.begin(9600);
#endif
    
    pinMode(SW, INPUT);
    pinMode(BUTTON, INPUT);

    attachPCINT(CLK);
    attachPCINT(DT);

    lcd.begin();
    lcd.backlight();
    lcd.blink_on();

    encoder.setType(TYPE2);
    encoder.setDirection(REVERSE);
    // sound.transfer();

    radio.setPacketFormat(2, 1);
    radio.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);
    radio.getRf24().setDataRate(RF24_250KBPS);

    radio2.setPacketFormat(2, 1);
    radio2.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);
    radio2.getRf24().setDataRate(RF24_250KBPS);

    updateScreen();
}

byte getADC(byte channel)
{
    if (channel > 3) channel = 0;

    Wire.beginTransmission(0x49);
    Wire.write(channel);
    Wire.endTransmission();

    Wire.requestFrom(0x49, 2);
    Wire.read();

    int value = Wire.read();

    return value;
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
    
    radio.setPacketValue((uint32_t)
                            value[0] * 1000 + 
                            value[1] *  100 +
                            value[2] *   10 +
                            value[3]); 

    radio.setPacketValue(analogRead(MICRO));

    radio2.sendPacket();
    radio.sendPacket();
        

    Serial.println(getADC(0));


    btnPrevState = btnCurrentState;
}

#elif MODE == SLAVE

void slave::setup()
{
    lcd.begin();
    lcd.backlight();
    lcd.blink_off();
    sound.reader();

    radio.begin(0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL);
    radio.setPacketFormat(2, 1);

    radio2.begin(0xF0F1F2F3F4LL, 0xF0F1F2F3F0LL);
    radio2.setPacketFormat(2, 1);

    pinMode(9, OUTPUT); 
}

void slave::loop()
{
    if (radio.receivePacket()) {
        uint32_t data = radio.getPacketValue(0);

        if (data <= 9999 || data >= 0){
            buffer = data;
        }

        updateScreen();
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