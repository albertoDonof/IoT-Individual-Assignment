# IoT-Individual-Assignment

Individual Assignment for Internet of Things - Algorithms and Services course

## The assignment
The requirements needed to solve the assignment are:

- Identify the maximum sampling frequency of the device
- Compute correctly the max freq of the input signal
- Compute correctly the optimal freq of the input signal
- Compute correctly the aggregate function over a window
- Evaluate correctly the saving in energy
- Evaluate correctly the communication cost
- Evaluate correctly the end-to-end latency
- Transmit the result to the edge server via MQTT+WIFI 
- Transmit the result to the cloud server via LoRaWAN + TTN 


## Technical Details

### Input Signal
For this assignment i choosed to simulate the input signal with the firmware of my esp32 board. The input signal i generated is composed by two sine waves with different amplitudes and frequencies and it's the following:
```
input_signal(t) = 2*sin(2*pi*3*t)+4*sin(2*pi*5*t)
```
### Maximum Sampling Frequency
The Maximum Sampling Frequency of my esp32 board depends on how we are obtaining the signal. For example if we use the ADC to sample the signal then the maximum sampling frequency will be bound to the ADC's sampling rate. In my case i generate the signal internally in the device so to simulate a sampling rate of a physical device i used a delay that is the minimum delay that can be used in our machine that is: 

```
vTaskDelay(pdMS_TO_TICKS(portTICK_PERIOD_MS));
```

where portTICK_PERIOD_MS is bound to freeRTOS settings and can be also improved but for me the default value is 1ms. With this in mind i simulated the sampling of a signal and stored the start time and end time of this compution and then to find the maximum frequency possible:

```
int maximum_frequency = (int)(count / ((end_time - start_time) / 1000000.0));
```

I divided by 1000000.0 because the functions to get the time gives a microseconds value and i need to convert it in seconds.

![alt text](images/image.png)

The result of this computation is 1000Hz that corrsiponds to the simulated maximum frequency of my device.

### Identify optimal Sampling Frequency

To identify the maximum frequency of our input signal , since it's a sum of multiple sines we have to compute the maximum frequency ourself knowing the magnitudes because the FFT.majorPeak returns an estimation of the dominant but we need to find the highest frequency with a good amount of magnitude that can be considered the maximum frequency component of the input signal.

### Compute aggregate function over a window


## Setup Guide
