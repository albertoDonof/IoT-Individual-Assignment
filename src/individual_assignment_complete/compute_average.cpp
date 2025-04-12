#include "compute_average.h"
#include "variables.h"

const uint8_t amplitude = getAmplitude();
uint16_t time_window_average = 5;

void aggregateFunction(void *PvParameters){
  float average_value = 0.0;

  Serial.print("Current Sampling Frequency: ");
  Serial.println(getSamplingFrequency() ,2);

  while(1){
    
    Serial.println("Starting to compute average ...");
    average_value = computeAverageOverWindow(getSamplingFrequency(), time_window_average);

    // Send to queue (block if full)
    if(xQueueSend(averageValueQueue, &average_value, portMAX_DELAY) != pdPASS) {
      Serial.println("Failed to send to queue!");
    }
    
    vTaskDelay(pdMS_TO_TICKS(10000)); // 100ms delay
  }
}

float computeAverageOverWindow(double samplingFrequency, uint16_t time_window){
  int samples = (int)(samplingFrequency * time_window);

  float delay_between_samples = (1/samplingFrequency) * 1000; //in milliseconds

  //double inputSignal[samples];

  /* Input signal: 2*sin(2π*3*t) + 4*sin(2π*5*t) */
  double ratio3 = 2 * PI * 3 / samplingFrequency;  // Fraction of a complete cycle stored at each sample (in radians)
  double ratio5 = 2 * PI * 5 / samplingFrequency;


  double sum = 0;
  int count = 0;
  for (uint16_t i = 0; i < samples; i++) {
    double sample = (amplitude * (2.0 * sin(i * ratio3) + 4.0 * sin(i * ratio5)) ); /* Build data with positive and negative values*/

    sum += sample;
    count++;

    delay( delay_between_samples );
  }
  // the average value over the window
  float average = sum / count;
  Serial.print("Average Value over a window of 5s: ");
  Serial.println(average, 2);

  return average;
}

