#ifndef SLAVE_H_
#define SLAVE_H_

#include <Arduino.h>

#include "SerialFlow.h"
#include "liquid_crystal_i2c.h"

#define CE      5
#define CS      6
#define SCRADDR 0x27

namespace slave
{
    SerialFlow radio(CE, CS);
    LiquidCrystal lcd(SCRADDR, 16, 2);

    uint32_t buffer;

    void setup();
    void loop();

    void updateScreen();
}

#define SLAVE slave

#endif