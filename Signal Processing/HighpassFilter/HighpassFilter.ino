/*
  Highpass Filter
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLING_RATE = 500;
const unsigned long BAUD_RATE = 115200;
const float SIGNAL_FREQUENCY = 75;
const float NOISE_FREQUENCY = 5;

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  static unsigned long timePast = 0;
  unsigned long timePresent = micros();
  unsigned long timeInterval = timePresent - timePast;
  timePast = timePresent;

  static long timer = 0;
  timer -= timeInterval;
  if (timer < 0) {
    timer += 1000000 / SAMPLING_RATE;
    double t = micros() / 1.0e6;

    double inputSignal = sin(2 * PI * SIGNAL_FREQUENCY * t) + 0.5 * sin(2 * PI * NOISE_FREQUENCY * t);
    double filteredSignal = HighpassFilter(inputSignal);

    Serial.print(inputSignal);
    Serial.print(",");
    Serial.println(filteredSignal);
  }
}

double HighpassFilter(double x_n) {
  /*
    Highpass Filter, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 500 Hz, Fc = 50 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  const int FILTER_ORDER = 4;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.4328, -1.7314, 2.5971, -1.7314, 0.4328 };
  double a[FILTER_ORDER + 1] = { 1.0000, -2.3695, 2.3140, -1.0547, 0.1874 };

  x[0] = x_n;

  for (int i = 0; i < FILTER_ORDER + 1; i++) {
    if (i == 0) {
      y[0] = b[i] * x[i];
    } else {
      y[0] += b[i] * x[i] - a[i] * y[i];
    }
  }

  for (int i = FILTER_ORDER; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}