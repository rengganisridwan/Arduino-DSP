/*
  Generate Sinewave
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

#include <MegunoLink.h>
TimePlot MyPlot;

const unsigned long kSamplingRateHz = 300;
const unsigned long kBaudRate = 500000;
const double kSignalFreqHz_1 = 1;
const double kSignalFreqHz_2 = 10;

void setup() {
  Serial.begin(kBaudRate);

  MyPlot.SetSeriesProperties("signal_1", Plot::Blue, Plot::Solid, 1, Plot::NoMarker);
  MyPlot.SetSeriesProperties("signal_2", Plot::Red, Plot::Solid, 1, Plot::NoMarker);
}

void loop() {
  static unsigned long past_time_us = 0;
  unsigned long present_time_us = micros();
  unsigned long time_interval_us = present_time_us - past_time_us;
  past_time_us = present_time_us;

  static double timer_us = 0;
  timer_us -= time_interval_us;
  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRateHz;

    double t = micros() / 1.0e6;
    int t_ms = millis();
    double input_signal_1 = sin(2 * PI * kSignalFreqHz_1 * t);
    double input_signal_2 = sin(2 * PI * kSignalFreqHz_2 * t);
    
    // String to_print = String(t_ms) + "," + String(input_signal_1) + "," + String(input_signal_2);
    // Serial.println(to_print);

    MyPlot.SendData("time (ms)",t_ms);
    MyPlot.SendData("signal 1",input_signal_1);
    MyPlot.SendData("signal 2",input_signal_2);
  }
}
