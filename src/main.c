#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "http_client.h"
#include "mqtt.h"

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
	char message[50];
	if(xSemaphoreTake(connectionMQTTSemaphore, portMAX_DELAY)){
		while(true){
			float temperatura = 20.0 + (float)rand()/(float)(RAND_MAX/10.0);
			sprintf(message, "temperatura1: %f", temperatura);
			mqttSendMessage("sensores/temperatura", message);
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
	}
}

void app_main(void){
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
