#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "frozen.c"
#include "mqtt_client.h"
#include "mqtt.h"
#include "ledRgb.h"

#define TAG "MQTT"

extern xSemaphoreHandle connectionMQTTSemaphore;
esp_mqtt_client_handle_t client;

void setLocalState(char *function, char value){
    if(strcmp(function, "setRedLed") == 0) GRed = value;
    else if(strcmp(function, "setBlueLed") == 0) GBlue = value;
    else if(strcmp(function, "setGreenLed") == 0) GGreen = value;
    else {
        printf("Function not found");
        return;
    }

    setColor(GRed, GGreen, GBlue);
}

void sendStoredState(char *path, int pathLength, char *function){
    char topic[40], state[4];
    sprintf(topic, "v1/devices/me/rpc/response/%.*s", pathLength - 26, path + 26);

    if(strstr(function, "RedLed") != NULL)
        sprintf(state, "%d", GRed);
    else if(strstr(function, "GreenLed") != NULL)
        sprintf(state, "%d", GGreen);
    else if(strstr(function, "BlueLed") != NULL)
        sprintf(state, "%d", GBlue);
    else {
        printf ("Function not found");
        return;
    }
    mqttSendMessage(topic, state);
}

static esp_err_t mqttEventHandlerCb(esp_mqtt_event_handle_t event){
    esp_mqtt_client_handle_t client = event->client;
    int msg_id, temp;
    char function[12];

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(connectionMQTTSemaphore);
            msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            json_scanf(event->data, event->data_len, "{method: %s, params: %d}", &function, &temp);
            if(strstr(function, "set") != NULL)
                setLocalState(function, temp);
            else if(strstr(function, "get") != NULL)
                sendStoredState(event->topic, event->topic_len, function);
            else
                printf("function %s not found\n", function);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqttEventHandlerCb(event_data);
}

void mqttStart(){
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://164.41.98.25",
        .username = "gWS6C6a1NWYMwVNelIbe",
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqttEventHandler, client);
    esp_mqtt_client_start(client);
}

void mqttSendMessage(char * topic, char * message){
    int message_id = esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
}