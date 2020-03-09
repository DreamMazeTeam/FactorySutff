#include "master.h"

void master::setup()
{
    pinMode(SW, INPUT);
    pinMode(BUTTON, INPUT);

    attachPCINT(CLK);
    attachPCINT(DT);

    lcd.begin();
    lcd.backlight();
    lcd.blink_on();

    encoder.setType(TYPE2);
    encoder.setDirection(REVERSE);

    radio.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);
    radio.setPacketFormat(2, 1);
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
                        (value[3] * 1000 +
                         value[2] *  100 +
                         value[1] *   10 +
                         value[0]));
    radio.sendPacket();

    btnPrevState = btnCurrentState;
}

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

ISR(PCINT0_vect) {
    //enc1.tick();
}
ISR(PCINT1_vect) {
    //enc1.tick();
}
ISR(PCINT2_vect) {
    master::encoder.tick();
}