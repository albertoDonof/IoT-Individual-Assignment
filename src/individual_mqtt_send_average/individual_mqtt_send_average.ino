
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "PubSubClient.h"
#include "secrets.h"


const char* ip_address_mqtt_Broker = "192.168.101.78";
const char* mqttServer = "192.168.101.78";
int port = 8883;
long lastMsg = 0;
String stMac;
char mac[50];
char clientId[50];

WiFiClientSecure espClient;
PubSubClient client(espClient);

float average_value = 0.0;
int result= 0;

bool waitingForResponse = false;
unsigned long pingSentTime = 0;
const char* pingTopic = "donofrio/individual/ping";

#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

void round_trip_time(void *pvParameters){
  while(1){
    
    if (!client.connected()) {
      mqttReconnect();
    }
    client.loop();

    if (!waitingForResponse && millis() % 5000 == 0) { // Send ping every 5 seconds
      char timestamp[20];
      unsigned long now = millis();
      ltoa(now, timestamp, 10);
    
      if (client.publish(pingTopic, timestamp)) {
        pingSentTime = now;
        waitingForResponse = true;
        Serial.println("Ping sent");
      }
    }

    //esp_task_wdt_reset(); // Prevent watchdog trigger
    vTaskDelay(1);

  }
}

void mqtt_send_average(void *pvParameters) {
  while(1){
    delay(10);

    if (!client.connected()) {
      mqttReconnect();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > 10000) {
      lastMsg = now;
    
      snprintf (msg, MSG_BUFFER_SIZE, "%.2f", average_value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      result = client.publish("donofrio/individual/average", msg);
      
      //Serial.println(result);
    }
  }
  
}

void setup() {
  Serial.begin(115200);

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  wifiConnect();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);
  
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_cert); // Set client certificate
  espClient.setPrivateKey(client_key);   // Set client private key

  client.setServer(mqttServer, port);
  client.setCallback(callback);

  // Create FreeRTOS task
  xTaskCreate(mqtt_send_average, "mqtt_send_average" , 5012, NULL, 1, NULL);
  
  //xTaskCreate(round_trip_time, "round_trip_time" , 5012, NULL, 1, NULL);

}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    // to connect to adafruit
    // if (client.connect(clientId,SECRET_MQTT_USER, SECRET_MQTT_API_KEY)) {
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("donofrio/individual/average");
      client.subscribe("donofrio/individual/response");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String stMessage;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();

  // Handles the response message of the ping previously sent to chech the rtt and so the end to end latency
  if (String(topic) == "donofrio/individual/response") {
    unsigned long pingReceivedTime = millis();

    unsigned long receivedValue = stMessage.toInt();

    if (receivedValue == pingSentTime) {
      unsigned long rtt = pingReceivedTime - pingSentTime;
      Serial.print("Round-Trip Time: ");
      Serial.print(rtt);
      Serial.println(" ms");
      waitingForResponse = false;

    } else {
      Serial.println("Wrong Timestamp!");
    }    
    
  }
}

void loop() {
  
}
