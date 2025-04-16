#ifndef COMPUTE_AVERAGE
#define COMPUTE_AVERAGE

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

float computeAverageOverWindow(double samplingFrequency, uint16_t time_window, float (*signal_input)(float));
void aggregateFunction(void *PvParameters);
void aggregateFunctionSleep(void *PvParameters);

#endif