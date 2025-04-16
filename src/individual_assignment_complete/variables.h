#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>
#include "freertos/queue.h"

//float getMaximumSamplingFrequency();
//void setMaximumSamplingFrequency(float maxFreq);

#define Time_To_Sleep 10   //Time ESP32 will go to sleep (in seconds)
#define S_To_uS_Factor 1000000ULL      //Conversion factor for micro seconds to seconds 

extern QueueHandle_t averageValueQueue;
extern QueueHandle_t startLoraQueue;

void createQueues();

float input_signal(float t);
float input_signal_bonus1(float t);
float input_signal_bonus2(float t);

bool getIsFFTComputed();
void setIsFFTComputed();

float getSamplingFrequency();
void setSamplingFrequency(float newFreq);

float getOriginalSamplingFrequency();
void setOriginalSamplingFrequency(float newFreq);

uint8_t getAmplitude();

#endif