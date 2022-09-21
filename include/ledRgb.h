#ifndef LEDRGB_H
#define LEDRGB_H

#include <stdint.h>

static uint8_t GRed = 0;
static uint8_t GGreen = 0;
static uint8_t GBlue = 0;

#define RGB_LIGHT_RED_GPIO 21
#define RGB_LIGHT_GREEN_GPIO 19
#define RGB_LIGHT_BLUE_GPIO 18

#define RGB_LED_CHANNEL_NUM 3

typedef struct
{
	int channel;
	int gpio;
	int mode;
	int timer_index;
}ledc_info_t;

void rgbInit();
void setColor(uint8_t red, uint8_t green, uint8_t blue);

#endif