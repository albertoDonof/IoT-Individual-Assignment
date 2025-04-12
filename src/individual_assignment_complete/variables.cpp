#include "variables.h"

QueueHandle_t averageValueQueue = NULL;

// Static variables (private to this file)
static volatile float currentSamplingFrequency = 0.0;  // Default 0
static volatile float originalSamplingFrequency = 0.0;

static const uint8_t amplitude = 10;

float getSamplingFrequency() {
  return currentSamplingFrequency;
}

void setSamplingFrequency(float newFreq) {
  if(newFreq > 0 ) {
    currentSamplingFrequency = round(newFreq);
    Serial.printf("Sampling frequency updated to: %.2f Hz\n", newFreq);
  }
}


float getOriginalSamplingFrequency() {
  return originalSamplingFrequency;
}

void setOriginalSamplingFrequency(float newFreq) {
  if(newFreq > 0 ) {
    originalSamplingFrequency = round(newFreq);
    Serial.printf("Original Sampling Frequency set to: %.2f Hz\n", newFreq);
  }
}

uint8_t getAmplitude(){
  return amplitude;
}

void createQueues() {
  // Create queue for average values (holds 5 float values)
  averageValueQueue = xQueueCreate(5, sizeof(float));
  
  if(averageValueQueue == NULL ) {
    Serial.println("Queue creation failed!");
  }
}
