/*
  Mean and Mean-Squared Estimate
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long kSamplingRate = 300;
const unsigned long kBaudRate = 500000;

const int kBufferLen = 150;
double mean_buffer[kBufferLen];
double meansq_buffer[kBufferLen];

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
    timer_us += 1e6 / kSamplingRate;
    double t = micros() / 1.0e6;

    double raw_signal = sin(2*PI*1*t)+1;
    double mean_est = MeanEstWithBuffer(raw_signal);
    double meansq_est = MeansqEstWithBuffer(raw_signal);
    double var = meansq_est - mean_est*mean_est;
    double stdev = sqrt(var);
    
    String to_print = String(raw_signal) + "," + String(stdev);
    Serial.println(to_print);
  }
}

double MeanEstimate(double x_n) {
  /*
    Estimating the mean value using IIR
    Reference: https://dsp.stackexchange.com/questions/811/determining-the-mean-and-standard-deviation-in-real-time
    Answered by nibot
  */
  const int kFilterOrder = 1;
  static double x[kFilterOrder + 1] = {};
  static double y[kFilterOrder + 1] = {};
  double a = 0.0111;

  x[0] = x_n;
  y[0] = (1-a) * y[1] - a * x[0];

  for (int i = kFilterOrder; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }
  return y[0];
}

double MeanEstWithBuffer(double x_n) {
  static int i;
  static double sum;

  sum -= mean_buffer[i];
  sum += x_n;

  mean_buffer[i] = x_n;
  i = (i + 1) % kBufferLen;

  return (sum/kBufferLen);
}

double MeansqEstWithBuffer(double x_n) {
  static int i;
  static double sum;

  sum -= meansq_buffer[i];
  sum += x_n*x_n;

  meansq_buffer[i] = x_n*x_n;
  i = (i + 1) % kBufferLen;

  return (sum/kBufferLen);
}