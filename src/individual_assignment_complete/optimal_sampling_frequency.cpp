#include "optimal_sampling_frequency.h"
#include "variables.h"


const uint16_t samples = 512; // Must be a power of 2
const uint8_t amplitude = getAmplitude();

double vReal[samples];
double vImag[samples];


#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void maxFrequencyInputSignal(float (*signal_input)(float), bool isBonus){
  double samplingFrequency = (double) getSamplingFrequency();

  ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

  float delay_between_samples = (1/samplingFrequency) * 1000; //in milliseconds

  /* Input signal: 2*sin(2π*3*t) + 4*sin(2π*5*t) */
  //double ratio3 = 2 * PI * 3 / samplingFrequency;  // Fraction of a complete cycle stored at each sample (in radians)
  //double ratio5 = 2 * PI * 5 / samplingFrequency;

  for (uint16_t i = 0; i < samples; i++) {
    float time = i / samplingFrequency;
    vReal[i] = (amplitude * signal_input(time)); /* Build data with positive and negative values*/
    vImag[i] = 0.0; // Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
  
    delay(delay_between_samples);
  }

  /* Print the results of the simulated sampling according to time */
  Serial.println("Data:");
  PrintVector(vReal, samples, SCL_TIME, samplingFrequency);
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); /* Weigh data */
  Serial.println("Weighed data:");
  PrintVector(vReal, samples, SCL_TIME, samplingFrequency);
  FFT.compute(FFTDirection::Forward); /* Compute FFT */
  Serial.println("Computed Real values:");
  PrintVector(vReal, samples, SCL_INDEX, samplingFrequency);
  Serial.println("Computed Imaginary values:");
  PrintVector(vImag, samples, SCL_INDEX, samplingFrequency);
  FFT.complexToMagnitude(); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY, samplingFrequency);

  /* Find max frequency */
  double maxFrequency = findLastPeakFrequencyWithInterpolation(vReal, (samples >> 1), samplingFrequency);
  Serial.print("Max Frequency (greatest frequency corresponding to a peak) of input signal: ");
  Serial.println(maxFrequency, 2);
  Serial.print("Optimal sampling Frequency: ");
  Serial.println( round(maxFrequency * 2) , 2);


  if(!isBonus){
    setSamplingFrequency( round(maxFrequency * 2) );

    setIsFFTComputed();
  }
  
    
  //vTaskDelay(10000000); // Repeat every 10 seconds
}

float findLastPeakFrequencyWithInterpolation(const double* magnitudes, int numBins, float samplingFreq) {
    //const float threshold = 0.3;
    const float threshold = 0.2;
    const int minPeakDistance = 2;
    
    float maxMagnitude = 0.0;
    float lastPeakFreq = 0.0;
    
    // Find global maximum
    for (int i = 0; i < numBins; i++) {
        if (magnitudes[i] > maxMagnitude) {
            maxMagnitude = magnitudes[i];
        }
    }
    
    // Find peaks with parabolic interpolation
    for (int i = 1; i < numBins - 1; i++) {

        // we check if this is a peak and if it's magnitude is at least > than 30% of the max magnitude
        if (magnitudes[i] > magnitudes[i-1] && magnitudes[i] > magnitudes[i+1] &&
            magnitudes[i] > (maxMagnitude * threshold)) {
            
            // Parabolic interpolation to find more precise peak location
            float alpha = magnitudes[i-1];
            float beta = magnitudes[i];
            float gamma = magnitudes[i+1];
            float p = 0.5f * (alpha - gamma) / (alpha - 2*beta + gamma);
            
            float interpolatedBin = i + p;
            float currentFreq = (interpolatedBin * samplingFreq) / (2 * numBins);
            
            // Only update if this peak is sufficiently far from previous
            if (lastPeakFreq == 0 || (currentFreq - lastPeakFreq) > ((minPeakDistance * samplingFreq) / (2 * numBins))) {
                lastPeakFreq = currentFreq;
            }
        }
    }
    
    return lastPeakFreq;
}


void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType, float samplingFreq)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
    break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFreq);
    break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFreq) / samples);
    break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

