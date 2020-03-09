#ifndef MASTER_H_
#define MASTER_H_

#include <Arduino.h>

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

#define VALUE_LEN 4

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
}

#define MASTER master

#endif