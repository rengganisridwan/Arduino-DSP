/*
  Setting Sampling Rate
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long kSamplingRateHz = 500;
const unsigned long kBaudRate = 115200;
const double kSignalFreqHz = 3;
const double kNoiseFreqHz = 50;

void setup() {
  Serial.begin(kBaudRate);
}

void loop() {
  // Calculate elapsed time
  static unsigned long past_time_us = 0;                            // Initiate static variable to store previous time
  unsigned long present_time_us = micros();                         // Obtain current time
  unsigned long time_interval_us = present_time_us - past_time_us;  // Calculate the elapsed time
  past_time_us = present_time_us;                                   // Store the current time as the previous time for the next loop

  // Subtract the timer by the elapsed time between subsequent loop. If the
  // timer value reaches zero, we do the measurement. Then, the timer value will
  // be added by the sampling time period. Thus, the measurement will be
  // conducted in a periodic manner.
  static long timer_us = 0;
  timer_us -= time_interval_us;
  if (timer_us < 0) {
    timer_us += 1000000 / kSamplingRateHz;
    double t = micros() / 1.0e6;

    double input_sig = sin(2 * PI * kSignalFreqHz * t);
    double noise_sig = 0.5 * sin(2 * PI * kNoiseFreqHz * t);
    double combined_sig = input_sig + noise_sig;
    Serial.println(combined_sig);
  }
}