#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>

/***************************************************************************
 *  Go to your TTN console register a device then the copy fields
 *  and replace the CHANGE_ME strings below
 ****************************************************************************/
const char* devEui = "70B3D57ED006F936"; // Change to TTN Device EUI
const char* appEui = "0000000000000000"; // Change to TTN Application EUI
const char* appKey = "9F40EC52BC056E61F6BE22EB57F9EDCA"; // Chaneg to TTN Application Key

double average_value = 0.0;

EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;
#ifndef AUTO_PIN_MAP // AUTO_PIN_MAP is set if board is defined in the file target-config.h
#include "board_config.h"
#endif // !AUTO_PINS


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

void loraSendAverage(void *pvParameters){
  while(1){

		// Cast the parameter to the original type
		double *data = (double*) pvParameters;
		double value = *data;

		lpp.reset();
		lpp.addDigitalInput(1, (int) value);
		if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
		{
			Serial.printf("Average value: %f TTN_CayenneLPP: %d %x %02X%02X\n", value, lpp.getBuffer()[0], lpp.getBuffer()[1],
				lpp.getBuffer()[2], lpp.getBuffer()[3]);
		}
    
    // Give some time before deleting the task (optional)
    vTaskDelay(5000); // Delay for 100 ms

  }

}

void setup()
{
	Serial.begin(115200);
	Serial.println("Starting");
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

	// Create FreeRTOS task
  xTaskCreate(loraSendAverage, "loraSendAverage" , 5012, &average_value, 1, NULL);
}

void loop()
{
	
}