#ifndef HALL_H
#define HALL_H

#include <stdio.h>
#include <unistd.h>
#include "driver/adc.h"

static const adc_channel_t channel = ADC2_CHANNEL_2;
static const adc_bits_width_t width = ADC_WIDTH_12Bit;
static int raw = 0;

#define HALL ADC2_CHANNEL_2
#define HALL_EFFECT_GPIO 2

void presentHall();

#endif