#include "driver/ledc.h"
#include "leds.h"
#include <stdint.h>

ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];
ledc_info_t internal_led;

void initInternalLed(){
	internal_led.channel = LEDC_CHANNEL_0;
	internal_led.gpio = INTERNAL_LIGHT_GPIO;
	internal_led.mode = LEDC_HIGH_SPEED_MODE;
	internal_led.timer_index = LEDC_TIMER_0;

	// Configuração do Timer
	ledc_timer_config_t timer_config = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = 1000,
		.clk_cfg = LEDC_AUTO_CLK
	};
	ledc_timer_config(&timer_config);

	// Configuração do Canal
	ledc_channel_config_t channel_config = {
		.gpio_num = INTERNAL_LIGHT_GPIO,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.timer_sel = LEDC_TIMER_0,
		.duty = 0,
		.hpoint = 0
	};
	ledc_channel_config(&channel_config);


}

void setIntensity(uint8_t intensity){
	ledc_fade_func_install(0);
	// //values shoud be 0 to 255
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, intensity);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

}

void rgbInit(){
	//RED
	ledc_ch[0].channel = LEDC_CHANNEL_0;
	ledc_ch[0].gpio = RGB_LIGHT_RED_GPIO;
	ledc_ch[0].mode = LEDC_HIGH_SPEED_MODE;
	ledc_ch[0].timer_index = LEDC_TIMER_0;

	//GREEN
	ledc_ch[1].channel = LEDC_CHANNEL_1;
	ledc_ch[1].gpio = RGB_LIGHT_GREEN_GPIO;
	ledc_ch[1].mode = LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].timer_index = LEDC_TIMER_0;

	//BLUE
	ledc_ch[2].channel = LEDC_CHANNEL_2;
	ledc_ch[2].gpio = RGB_LIGHT_BLUE_GPIO;
	ledc_ch[2].mode = LEDC_HIGH_SPEED_MODE;
	ledc_ch[2].timer_index = LEDC_TIMER_0;

	//CONDIGURE TIMER 0
	ledc_timer_config_t ledc_timer = {
		.duty_resolution = LEDC_TIMER_8_BIT,
		.freq_hz = 100,
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.timer_num = LEDC_TIMER_0
	};
	ledc_timer_config(&ledc_timer);

	//Configure channels

	for (int rgb_ch = 0; rgb_ch < RGB_LED_CHANNEL_NUM; rgb_ch++){

		ledc_channel_config_t ledc_channel = {
			.channel = ledc_ch[rgb_ch].channel,
			.duty = 0,
			.hpoint = 0,
			.gpio_num = ledc_ch[rgb_ch].gpio,
			.intr_type = LEDC_INTR_DISABLE,
			.speed_mode = ledc_ch[rgb_ch].mode,
			.timer_sel = ledc_ch[rgb_ch].timer_index,
		};
		ledc_channel_config(&ledc_channel);
	}

}

void setColor(uint8_t red, uint8_t green, uint8_t blue){
	//values shoud be 0 to 255
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}

void setGlobalColor(){
	//values shoud be 0 to 255
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, GRed);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, GGreen);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, GBlue);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}