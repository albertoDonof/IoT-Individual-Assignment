#ifndef OPTIMAL_SAMPLING_FREQUENCY
#define OPTIMAL_SAMPLING_FREQUENCY

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arduinoFFT.h>


//void maxFrequencyInputSignal(void *pvParameters);
void maxFrequencyInputSignal(float (*signal_input)(float), bool isBonus);

float findLastPeakFrequencyWithInterpolation(const double* magnitudes, int numBins, float samplingFreq);
void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType, float samplingFreq);

#endif