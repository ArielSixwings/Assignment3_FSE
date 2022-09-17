#include <stdio.h>
#include <dht11.h>
#include <string.h>
#include <nvs.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "http_client.h"
#include "mqtt.h"
#include "driver/ledc.h"
#include "driver/adc.h"

#define HALL_EFFECT_GPIO 2

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

ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];


static void rgbInit(){
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
		.duty_resolution =LEDC_TIMER_8_BIT,
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
			.timer_sel =ledc_ch[rgb_ch].timer_index,
		};
		ledc_channel_config(&ledc_channel);
	}

}

static void setColor(uint8_t red, uint8_t green, uint8_t blue){
	//values shoud be 0 to 255
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}

void testRgb(){
	rgbInit();
	int r = 0;
	int g = 0;
	int b = 0;

	for ( r = 0; r < 256; r++){
		setColor(r,g,b);
		vTaskDelay(10);
	}
	for ( g = 0; g < 256; g++)
	{
		setColor(r,g,b);
		vTaskDelay(10);
	}
	for ( b = 0; b < 256; b++)
	{
		setColor(r,g,b);
		vTaskDelay(10);
	}

}
xSemaphoreHandle connectionWifiSemaphore;
xSemaphoreHandle connectionMQTTSemaphore;

void connectedWifi(void * params){
	while(true){
		if(xSemaphoreTake(connectionWifiSemaphore, portMAX_DELAY)){
			// Processamento Internet
			mqttStart();
		}
	}
}

void handleServerConnection(void * params){
	// char message[50];
	char jsonMessage[200];
	if(xSemaphoreTake(connectionMQTTSemaphore, portMAX_DELAY)){
		while(true){

			sprintf(jsonMessage, "{\"temperature\": %d}", DHT11_read().temperature);
			mqttSendMessage("v1/devices/me/telemetry", jsonMessage);

			sprintf(jsonMessage, "{\"Humidity\": %d}", DHT11_read().humidity);
			mqttSendMessage("v1/devices/me/telemetry", jsonMessage);

			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

void app_main(void){
		testRgb();
    	DHT11_init(GPIO_NUM_23);
		// Initialize the NVS
		esp_err_t ret = nvs_flash_init();
		if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK(ret);

		printf("======= Testando sensor DHT11 =======\nTemp: %d\nUmidade: %d\n", DHT11_read().temperature, DHT11_read().humidity);

		connectionWifiSemaphore = xSemaphoreCreateBinary();
		connectionMQTTSemaphore = xSemaphoreCreateBinary();
		wifiStart();

		xTaskCreate(&connectedWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
		xTaskCreate(&handleServerConnection, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
