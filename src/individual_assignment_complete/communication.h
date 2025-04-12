#ifndef COMMUNICATION
#define COMMUNICATION

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "PubSubClient.h"

#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>
#ifndef AUTO_PIN_MAP // AUTO_PIN_MAP is set if board is defined in the file target-config.h
#include "board_config.h"
#endif // !AUTO_PINS

void mqtt_send_average(void* pvParameters);
void round_trip_time(void *pvParameters);

void setupMain();
void wifiConnect();
void wifiDisconnect();
void mqttReconnect();
void callback(char* topic, byte* message, unsigned int length);

void loraSendAverage(void *pvParameters);
void setupLoraConn();
void message(const uint8_t* payload, size_t size, uint8_t port, int rssi);

#endif