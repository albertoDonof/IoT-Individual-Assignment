#include <arduinoFFT.h>

const uint16_t samples = 512; // Must be a power of 2
const double samplingFrequency = 1000; // Hz
const uint8_t amplitude = 100;

double vReal[samples];
double vImag[samples];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

void maxFrequencyInputSignal(void *pvParameters){
  while(1){
    /* Input signal: 2*sin(2π*3*t) + 4*sin(2π*5*t) */
    double ratio3 = 2 * PI * 3 / samplingFrequency;  // Fraction of a complete cycle stored at each sample (in radians)
    double ratio5 = 2 * PI * 5 / samplingFrequency;

    for (uint16_t i = 0; i < samples; i++) {
      vReal[i] = (amplitude * (2.0 * sin(i * ratio3) + 4.0 * sin(i * ratio5)) / 2.0); /* Build data with positive and negative values*/
      vImag[i] = 0.0; // Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }


    /* Apply FFT */
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD); // Apply window function
    FFT.compute(FFT_FORWARD); // Compute FFT
    FFT.complexToMagnitude(); // Get magnitudes

    /* Find dominant frequency */
    double peakFrequency = FFT.majorPeak();
    Serial.print("Max Frequency of input signal: ");
    Serial.println(peakFrequency, 2);
    Serial.print("Optimal sampling Frequency: ");
    Serial.println( round(peakFrequency * 2) , 2);
    
    
    vTaskDelay(4000); // Repeat every second
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Ready");

  // Create FreeRTOS task
  xTaskCreate(maxFrequencyInputSignal, "maxFrequencyInputSignal" , 5012, NULL, 1, NULL);
}

void loop() {
}