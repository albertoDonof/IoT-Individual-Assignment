#include "variables.h"

QueueHandle_t averageValueQueue = NULL;
QueueHandle_t startLoraQueue = NULL;

// Static variables (private to this file)
static volatile RTC_DATA_ATTR float currentSamplingFrequency = 0.0;  // Default 0
static volatile RTC_DATA_ATTR float originalSamplingFrequency = 0.0;

static const uint8_t amplitude = 10;

static RTC_DATA_ATTR bool isFFTComputed = false;


bool getIsFFTComputed(){
  return isFFTComputed;
}

void setIsFFTComputed(){
  isFFTComputed = !isFFTComputed;
}

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

// Input signal: 2*sin(2π*3*t) + 4*sin(2π*5*t) 
float input_signal(float t){
  return 2.0 *sin(2 * PI * 3 * t) + 4.0 * sin( 2 * PI * 5 * t);
}

// Input signal: 
float input_signal_bonus1(float t){
  return 2.0 *sin(2 * PI * 10 * t) + sin( 2 * PI * 50 * t) + 5.0 * sin( 2 * PI * 8 * t);
}

float input_signal_bonus2(float t){
  return 2.0*sin(2 * PI * 3 * t) + 3.0 * sin( 2 * PI * 250 * t);
}

void createQueues() {
  // Create queue for average values (holds 5 float values)
  averageValueQueue = xQueueCreate(5, sizeof(float));

  startLoraQueue = xQueueCreate(1, sizeof(bool));
  
  if(averageValueQueue == NULL ) {
    Serial.println("Queue creation failed!");
  }
  if(startLoraQueue == NULL ) {
    Serial.println("Queue creation failed!");
  }
}
