#include "MQTT.h"
#include "MQTTClient.h"

static MQTTClient client;
static Network network;

bool MQTT_Init(uint8_t socket, uint16_t port)
{
    NewNetwork(&network, socket);
    MQTTClientInit(&client, &network, 1000, NULL, 2048, NULL, 2048);
}