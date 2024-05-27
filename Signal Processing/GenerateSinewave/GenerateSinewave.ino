const unsigned long kSamplingRateHz = 500;
const unsigned long kBaudRate = 115200;
const double kSignalFreqHz = 75;

void setup() {
  Serial.begin(kBaudRate);
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
    double input_sig = sin(2 * PI * kSignalFreqHz * t);
    double loop_exec_time_us = (micros() - present_time_us);
    
    String to_print = String(input_sig) + "," + String(loop_exec_time_us);
    Serial.println(to_print);
  }
}
