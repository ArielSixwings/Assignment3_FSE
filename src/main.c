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
#include "ledRgb.h"
#include "driver/adc.h"


static const adc_channel_t channel = ADC2_CHANNEL_2;
static const adc_bits_width_t width = ADC_WIDTH_12Bit;

// static const adc_atten_t atten = ADC_ATTEN_DB_0;
// static const adc_unit_t unit = ADC_UNIT_2;

#define HALL ADC2_CHANNEL_2
#define HALL_EFFECT_GPIO 2

#define RGB_LIGHT_RED_GPIO 21
#define RGB_LIGHT_GREEN_GPIO 19
#define RGB_LIGHT_BLUE_GPIO 18

#define RGB_LED_CHANNEL_NUM 3

void testHall(){
	gpio_pad_select_gpio(HALL_EFFECT_GPIO);
	gpio_set_direction(HALL_EFFECT_GPIO, GPIO_MODE_INPUT);

	// Configura o conversor AD
	adc1_config_width(ADC_WIDTH_BIT_10);
	adc1_config_channel_atten(HALL, ADC_ATTEN_MAX);
	// adc2_config_channel_atten((adc2_channel_t)channel,atten);


	int raw = 0;
	for (size_t i = 0; i < 100; i++){
		raw = 0;
		adc2_get_raw((adc2_channel_t)channel, width,&raw);
		printf("HALL EFFECT: %d\n", raw);
		fflush(stdout);
		vTaskDelay(200);
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

void app_main(void){
		// testHall();
		// testRgb();
		rgbInit();
    	DHT11_init(GPIO_NUM_23);
		// Initialize the NVS
		esp_err_t ret = nvs_flash_init();
		if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK(ret);

		connectionWifiSemaphore = xSemaphoreCreateBinary();
		connectionMQTTSemaphore = xSemaphoreCreateBinary();
		wifiStart();

		xTaskCreate(&connectedWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
		xTaskCreate(&handleServerConnection, "Comunicação com Broker", 4096, NULL, 1, NULL);
}