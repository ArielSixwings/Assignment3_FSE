#ifndef LIGHTSLEEP_H
#define LIGHTSLEEP_H

#include "driver/rtc_io.h"

#define WAKE_UP_BUTTON 0

void lightSleepInit();
void setSleepMode();

#endif