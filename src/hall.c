#include "hall.h"

void presentHall(){
	gpio_pad_select_gpio(HALL_EFFECT_GPIO);
	gpio_set_direction(HALL_EFFECT_GPIO, GPIO_MODE_INPUT);

	// Configura o conversor AD
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(HALL, ADC_ATTEN_MAX);
	// adc2_config_channel_atten((adc2_channel_t)channel,atten);

	for (size_t i = 0; i < 1; i++){
		adc2_get_raw((adc2_channel_t)channel, width,&raw);
		printf("HALL EFFECT: %d\n", raw);
		fflush(stdout);
		usleep(2000000);
	}

}