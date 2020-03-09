#include <Arduino.h>

#include "slave.h"
#include "master.h"

#define mode MASTER

void setup() {
    mode::setup();
}

void loop() {
    mode::loop();
}