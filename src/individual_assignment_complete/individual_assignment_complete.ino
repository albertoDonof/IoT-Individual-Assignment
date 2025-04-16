#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arduinoFFT.h>


// My header files:
//#include "secrets.h"
#include "variables.h"
#include "max_sampling_frequency.h"
#include "optimal_sampling_frequency.h"
#include "compute_average.h"
#include "communication.h"

bool doingBonus = true;
bool doingDeepSleepMode = true; 

bool doingLora = false; // if doing sleep mode is set to true than this is ignored
bool useOverSamplingFrequency = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Ready: ");

  delay(100); //Takes time to start the serial monitor

  createQueues();

  if( getIsFFTComputed()==false){
    maxSamplingFrequency();
  
    delay(10);

    if(doingBonus){

      Serial.println("Start FFT computation for signal bonus1 2.0 *sin(2 * PI * 10 * t) + sin( 2 * PI * 50 * t) + 5.0 * sin( 2 * PI * 8 * t) : ");
      maxFrequencyInputSignal(input_signal_bonus1, true);

      Serial.println("Start FFT computation for signal bonus2 2.0*sin(2 * PI * 3 * t) + 3.0 * sin( 2 * PI * 250 * t) : ");
      maxFrequencyInputSignal(input_signal_bonus2, true);
    }

    if(!useOverSamplingFrequency){
      maxFrequencyInputSignal(input_signal, false);
    }
  
    delay(10);  

  }
  

  if(doingDeepSleepMode){

    xTaskCreate( aggregateFunctionSleep, "aggregateFunctionSleep", 2048 , NULL, 2 ,NULL);
    
    xTaskCreate( mqtt_send_average_sleep, "mqtt_send_average_sleep", 5012, NULL, 1 , NULL);

  } else{

    xTaskCreate( aggregateFunction, "aggregateFunction", 2048 , NULL, 2 ,NULL);

    if(doingLora){
      
      xTaskCreate( loraSendAverage, "loraSendAverage", 5012, NULL, 1 , NULL);
    } else{
      xTaskCreate( mqtt_send_average, "mqtt_send_average", 5012, NULL, 1 , NULL);
    }

    
  }
  

}

void loop() {
  vTaskDelay(portMAX_DELAY); // FreeRTOS tasks handle execution
}
