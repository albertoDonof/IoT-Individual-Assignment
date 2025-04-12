#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>
#include <queue.h>

//float getMaximumSamplingFrequency();
//void setMaximumSamplingFrequency(float maxFreq);

extern QueueHandle_t averageValueQueue;

void createQueues();

float getSamplingFrequency();
void setSamplingFrequency(float newFreq);

float getOriginalSamplingFrequency();
void setOriginalSamplingFrequency(float newFreq);

uint8_t getAmplitude();

#endif