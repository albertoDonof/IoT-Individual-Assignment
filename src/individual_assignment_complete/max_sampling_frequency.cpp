#include "max_sampling_frequency.h"
#include "variables.h"

const uint16_t samples_signal = 10000;
double signal_dig[samples_signal];

void maxSamplingFrequency(){
      // Obtain the start time in microseconds
  int64_t start_time = esp_timer_get_time();

  // Simulate sampling by iterating through the signal data array and counting the number of samples iterated
  int count = 0;
  for (int i = 0; i < samples_signal; i++) {
    // Simulate sampling by reading the data
    float sample = signal_dig[i];
    count++;
    // Delay by the minimum delay between samples (portTICK_PERIOD_MS) to simulate the maximum sampling frequency
    vTaskDelay(pdMS_TO_TICKS(portTICK_PERIOD_MS));
  }

  // Obtain the end time in microseconds
  int64_t end_time = esp_timer_get_time();

  float maximum_frequency = (count / ((end_time - start_time) / 1000000.0));

  setOriginalSamplingFrequency(maximum_frequency);
  setSamplingFrequency(maximum_frequency);
      

  // Calculate the maximum sampling frequency:
  // Divide the total count of samples by the elapsed time (in seconds) to get the frequency in Hz, remembering that the time is in microseconds
  Serial.print("Maximum Sampling Frequency: ");
  Serial.print((int) maximum_frequency);
  Serial.println("Hz");
  
}