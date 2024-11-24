
const unsigned long kSamplingRateHz = 10;
const unsigned long kBaudRate = 115200;
const double kSignalFreqHz = 1;

void setup() {
  Serial.begin(kBaudRate);
}

void loop() {
  static int buffer_idx = 0;
  const int kBufferSize = 10;
  static double input_sig_buffer[kBufferSize];
  static double t_ms_buffer[kBufferSize];

  static unsigned long past_time_us = 0;
  unsigned long present_time_us = micros();
  unsigned long time_interval_us = present_time_us - past_time_us;
  past_time_us = present_time_us;

  static double timer_us = 0;
  timer_us -= time_interval_us;
  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRateHz;

    double t = micros() / 1.0e6;
    unsigned long t_ms = millis();
    double input_sig = sin(2 * PI * kSignalFreqHz * t);
    // double loop_exec_time_us = (micros() - present_time_us);
    
    // String to_print = String(t_ms) + "," + String(input_sig); 
    // String to_print = String(input_sig) + "," + String(loop_exec_time_us);
    // Serial.println(to_print);
    // MyPlot.SendData("signal",t_ms,input_sig);

    input_sig_buffer[buffer_idx] = input_sig;
    t_ms_buffer[buffer_idx] = t_ms;
    buffer_idx = (buffer_idx + 1) % kBufferSize;
    if (buffer_idx == 0) {
      String to_print = "";
      for (int i = 0; i < kBufferSize; i++) {
        if (i == kBufferSize-1) {
          to_print += String(t_ms_buffer[i]) + "," + String(input_sig_buffer[i]);
        } else {
          to_print += String(t_ms_buffer[i]) + "," + String(input_sig_buffer[i]) + ";"; 
        // Serial.println(to_print);
        }
      }
      Serial.println(to_print);
      to_print = "";
    }
  }
}
