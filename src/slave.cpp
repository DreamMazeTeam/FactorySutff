#include "slave.h"

void slave::setup()
{
    lcd.begin();
    lcd.backlight();
    lcd.blink_off();

    radio.begin(0xF0F0F0F0D2LL,0xF0F0F0F0E1LL);
    radio.setPacketFormat(2, 1);
}

void slave::loop()
{
    if (radio.receivePacket()) {
        buffer = radio.getPacketValue(0);
        updateScreen();
    }
}

void slave::updateScreen()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buffer);
}