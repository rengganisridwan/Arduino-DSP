/*
  Setting Sampling Rate
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/
#include <MegunoLink.h>

const unsigned long kSamplingRateHz = 300;
const unsigned long kBaudRate = 500000;
const double kSignalFreqHz = 1;
const double kNoiseFreqHz = 50;

TimePlot MyPlot;

void setup() {
  Serial.begin(kBaudRate);
}

void loop() {
  // Calculate elapsed time
  static unsigned long t_past_us = 0;
  unsigned long t_present_us = micros();
  unsigned long t_interval_us = t_present_us - t_past_us;
  t_past_us = t_present_us;

  // Subtract the timer by the elapsed time between subsequent loop. If the 
  // timer value reaches zero, we do the measurement. Then, the timer value 
  // will be added by the sampling time period. Thus, the measurement will be 
  // conducted in a periodic manner.
  static long timer_us = 0;
  timer_us -= t_interval_us;
  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRateHz;
    double t = micros() / 1.0e6;

    double input_signal = sin(2 * PI * kSignalFreqHz * t);
    double noise_signal = 0.5 * sin(2 * PI * kNoiseFreqHz * t);
    double combined_signal = input_signal + noise_signal;
    
    // MyPlot.SendData("t",millis());
    MyPlot.SendData("CH-01",combined_signal);
  }
}