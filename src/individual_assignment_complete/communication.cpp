#include "communication.h"
#include "secrets.h"
#include "variables.h"

// LORA connection setup
EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;

bool isLoraConnected = false;

// End LORA connection setup

// MQTT connection setup 
const char* ip_address_mqtt_Broker = "192.168.101.78";
const char* mqttServer = "192.168.101.78";
int port = 8883;
long lastMsg = 0;
String stMac;
char mac[50];
char clientId[50];

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

bool waitingForResponse = false;
unsigned long pingSentTime = 0;
const char* pingTopic = "donofrio/individual/ping";

bool isConnected = false;
int result= 0;
// End MQTT connection setup


/*  START MQTT FUNCTIONS  */

void mqtt_lora_send_average(void* pvParameters) {
  delay(10);
  float average_value = 0.0;

   while(1) {
    // Receive from average queue (block if empty)
    if(xQueueReceive(averageValueQueue, &average_value, portMAX_DELAY) == pdTRUE) {
      
      if (!isConnected || WiFi.status() != WL_CONNECTED) {
        setupMain();
        isConnected = true;
      }

      if (!client.connected()) {
        mqttReconnect();
      }
      //client.loop();

      long now = millis();
      if (now - lastMsg > 1000 ) {
        lastMsg = now;

        snprintf(msg, MSG_BUFFER_SIZE, "%.2f", average_value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        result = client.publish("donofrio/individual/average", msg);

        client.disconnect();        
        //Serial.println(result);
        WiFi.disconnect();
      }    

      vTaskDelay(100 );

      if (!isLoraConnected) {
        setupLoraConn();
        isLoraConnected = true;
      }

      lpp.reset();
      lpp.addDigitalInput(1, (int) average_value);
      if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
      {
        Serial.printf("Average value: %f TTN_CayenneLPP: %d %x %02X%02X\n", (double) average_value, lpp.getBuffer()[0], lpp.getBuffer()[1],
          lpp.getBuffer()[2], lpp.getBuffer()[3]);
      }
      
      vTaskDelay(2000 );

    }
  }
  
}

void mqtt_send_average(void* pvParameters) {
  delay(10);
  float average_value = 0.0;

   while(1) {
    // Receive from average queue (block if empty)
    if(xQueueReceive(averageValueQueue, &average_value, portMAX_DELAY) == pdTRUE) {
      
      if (!isConnected || WiFi.status() != WL_CONNECTED) {
        setupMain();
        isConnected = true;
      }

      if (!client.connected()) {
        mqttReconnect();
      }
      //client.loop();

      long now = millis();
      if (now - lastMsg > 1000 ) {
        lastMsg = now;

        snprintf(msg, MSG_BUFFER_SIZE, "%.2f", average_value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        result = client.publish("donofrio/individual/average", msg);

        client.disconnect();        
        //Serial.println(result);
      }    
      
    }
  }
  
}

void mqtt_send_average_sleep(void* pvParameters) {
  delay(10);
  float average_value = 0.0;

   while(1) {
    // Receive from average queue (block if empty)
    if(xQueueReceive(averageValueQueue, &average_value, portMAX_DELAY) == pdTRUE) {
      
      if (!isConnected || WiFi.status() != WL_CONNECTED) {
        setupMain();
        isConnected = true;
      }

      if (!client.connected()) {
        mqttReconnect();
      }
      //client.loop();

      long now = millis();
      if (now - lastMsg > 1000 ) {
        lastMsg = now;

        snprintf(msg, MSG_BUFFER_SIZE, "%.2f", average_value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        result = client.publish("donofrio/individual/average", msg);

        client.disconnect();        
        //Serial.println(result);
        enter_deep_sleep();

        vTaskDelete(NULL);
      }    
      
    }
  }
}

void enter_deep_sleep(){
   //Set timer to 10 seconds
  esp_sleep_enable_timer_wakeup(Time_To_Sleep * S_To_uS_Factor);
  Serial.println("Setup ESP32 to sleep for " + String(Time_To_Sleep) +
  " Seconds");

  //Go to sleep now
  esp_deep_sleep_start();
}

void round_trip_time(void *pvParameters){
  while(1){

    if (!isConnected || WiFi.status() != WL_CONNECTED) {
      setupMain();
      isConnected = true;
    }
    
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
    vTaskDelay(10);

  }
}


//setup wifi connection and mqtt broker connection
void setupMain() {
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
  espClient.setCertificate(client_cert);  // Set client certificate
  espClient.setPrivateKey(client_key);    // Set client private key

  client.setServer(mqttServer, port);
  //client.setCallback(callback);
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void wifiDisconnect(){
  while ( WiFi.status() == WL_CONNECTED){
    WiFi.disconnect();
    Serial.println("WiFi disconnected");
  }
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      //client.subscribe("donofrio/individual/average");
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

/*  END MQTT FUNCTIONS  */


/*  START LORA FUNCTIONS  */

void loraSendAverage(void *pvParameters){
  float average_value = 0.0;
  
  while(1){

    // Receive from average queue (block if empty)
    if(xQueueReceive(averageValueQueue, &average_value, portMAX_DELAY) == pdTRUE) {

      if (!isLoraConnected) {
        setupLoraConn();
        isLoraConnected = true;
      }

      lpp.reset();
      lpp.addDigitalInput(1, (int) average_value);
      if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
      {
        Serial.printf("Average value: %f TTN_CayenneLPP: %d %x %02X%02X\n", (double) average_value, lpp.getBuffer()[0], lpp.getBuffer()[1],
          lpp.getBuffer()[2], lpp.getBuffer()[3]);
      }
      
    }
    
    //vTaskDelay(10000); // Delay for 10 s

  }
}

void setupLoraConn(){
  #ifndef AUTO_PIN_MAP
	SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
  #endif // !AUTO_PIN_MAP
	ttn.begin();
	ttn.onMessage(message); // Declare callback function for handling downlink
	// messages from server
	ttn.join(devEui, appEui, appKey);
	Serial.print("Joining TTN ");
	while (!ttn.isJoined())
	{
		Serial.print(".");
		delay(500);
	}
	Serial.println("\njoined !");
	ttn.showStatus();
}

void message(const uint8_t* payload, size_t size, uint8_t port, int rssi)
{
	Serial.println("-- MESSAGE");
	Serial.printf("Received %d bytes on port %d (RSSI=%ddB) :", size, port, rssi);
	for (int i = 0; i < size; i++)
	{
		Serial.printf(" %02X", payload[i]);
	}
	Serial.println();
}

/*  END LORA FUNCTIONS  */