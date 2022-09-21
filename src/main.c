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
#include "leds.h"
#include "driver/ledc.h"
#include "hall.h"
#include "sleepMode.h"
#include "mynvs.h"

xSemaphoreHandle connectionWifiSemaphore;
xSemaphoreHandle connectionMQTTSemaphore;

#define LED_PIN 2

void led_blink(){
	gpio_pad_select_gpio(LED_PIN);
	gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);

	while (1){
		gpio_set_level(LED_PIN,0);
		vTaskDelay(1000/portTICK_RATE_MS);
		gpio_set_level(LED_PIN,1);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void connectedWifi(void * params){
	while(true){
		if(xSemaphoreTake(connectionWifiSemaphore, portMAX_DELAY)){
			// Processamento Internet
			mqttStart();
		}
	}
}

void handleServerConnection(void * params){
	char jsonMessage[200];
	int temperature = -1, humidity = -1;

	if(xSemaphoreTake(connectionMQTTSemaphore, portMAX_DELAY)){
		while(true){
			while (temperature < 0) temperature = DHT11_read().temperature;
			while (humidity < 0) humidity = DHT11_read().humidity;

			sprintf(jsonMessage, "{\"Temperature\": %d}", temperature);
			mqttSendMessage("v1/devices/me/telemetry", jsonMessage);

			sprintf(jsonMessage, "{\"Humidity\": %d}", humidity);
			mqttSendMessage("v1/devices/me/telemetry", jsonMessage);

			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

void getLastState(){
	initInternalLed();
	rgbInit();

	int value = 0;

	value = readNvsValue("RED");
	if(value > 0) GRed = value;
	printf("RED: %d\n",GRed);

	value = readNvsValue("GREEN");
	if(value > 0) GGreen = value;
	printf("GGreen: %d\n",GGreen);

	value = readNvsValue("BLUE");
	if(value > 0) GBlue = value;
	printf("GBlue: %d\n",GBlue);

	value = readNvsValue("PWM");
	if(value > 0 ){
		setIntensity(value);
	} else{
		setIntensity(0);
	}
	// printf("PWM: %d\n",PWM);

	fflush(stdout);
	setColor(GRed, GGreen, GBlue);
	getGlobal(GRed, GGreen, GBlue);

}
void app_main(void){
	// Initialize the NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	getLastState();
	lightSleepInit();
	// presentHall();
    DHT11_init(GPIO_NUM_23);


	connectionWifiSemaphore = xSemaphoreCreateBinary();
	connectionMQTTSemaphore = xSemaphoreCreateBinary();
	wifiStart();

	// xTaskCreate(&presentHall, "Apresenta o Hall", 4096, NULL, 1, NULL);

	// xTaskCreate(&led_blink,  "Blink", 512, NULL, 5, NULL);
	xTaskCreate(&connectedWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
	xTaskCreate(&handleServerConnection, "Comunicação com Broker", 4096, NULL, 1, NULL);

}