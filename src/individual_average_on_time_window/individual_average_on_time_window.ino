#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const double samplingFrequency = 11; // Optimal Sampling Frequency calculated before in Hz
const uint16_t time_window = 5; //seconds
const uint8_t amplitude = 100;

void averageFunctionOnWindow(void *pvParameters){
  while(1){
    int samples = (int)(samplingFrequency * time_window);

    double inputSignal[samples];

    /* Input signal: 2*sin(2π*3*t) + 4*sin(2π*5*t) */
    double ratio3 = 2 * PI * 3 / samplingFrequency;  // Fraction of a complete cycle stored at each sample (in radians)
    double ratio5 = 2 * PI * 5 / samplingFrequency;


    double sum = 0;
    int count = 0;
    for (uint16_t i = 0; i < samples; i++) {
      double sample = (amplitude * (2.0 * sin(i * ratio3) + 4.0 * sin(i * ratio5)) / 2.0); /* Build data with positive and negative values*/

      sum += sample;
      count++;
    }
    // the average value over the window
    double average = sum / count;
    Serial.print("Average Value over a window of 5s: ");
    Serial.println(average, 6);
    
    // Give some time before deleting the task (optional)
    vTaskDelay(20000); // Delay for 100 ms

  }
  
  

}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Ready: ");
  
  // Create FreeRTOS task
  xTaskCreate(averageFunctionOnWindow, "averageFunctionOnWindow" , 5012, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(portMAX_DELAY); // FreeRTOS tasks handle execution
}


