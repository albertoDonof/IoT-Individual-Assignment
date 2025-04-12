#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arduinoFFT.h>


// My header files:
//#include "secrets.h"
#include "variables.h"
#include "max_sampling_frequency.h"
#include "optimal_sampling_frequency.h"
#include "compute_average.h"
#include "communication.h"


/*
To do:  a task to compute average in fixed time window , then send the average through mqtt or lora and finally 
enter deep sleep with a timer of like 10 seconds , and after start again computing the average because before we saved 
in rtc variables if we have already done FFT and maxSampling Frequency since that will not be resetted as the other variables
*/

void setup() {
  Serial.begin(115200);
  Serial.println("Ready: ");

  delay(100); //Takes time to start the serial monitor

  createQueues();


  maxSamplingFrequency();
  
  delay(10);
  maxFrequencyInputSignal();

  delay(10);
  
  xTaskCreate( aggregateFunction, "aggregateFunction", 2048 , NULL, 2 ,NULL);

  xTaskCreate( mqtt_send_average, "mqtt_send_average", 5012, NULL, 1 , NULL);

}

void loop() {
  vTaskDelay(portMAX_DELAY); // FreeRTOS tasks handle execution
}
