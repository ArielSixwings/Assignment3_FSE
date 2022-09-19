#ifndef MQTT_H
#define MQTT_H

void mqttStart();
// void mqttSubscribe(char * topic);
void mqttSendMessage(char * topic, char * message);

#endif