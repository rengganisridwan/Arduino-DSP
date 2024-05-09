/*
  Lowpass Filter
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLING_RATE = 500;  // If this value changed, filter coefficients must be recalculated
const unsigned long BAUD_RATE = 115200;
const float SIGNAL_FREQUENCY = 3;
const float NOISE_FREQUENCY = 50;

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  // Calculate elapsed time
  static unsigned long timePast = 0;                    // Initiate static variable to store previous time
  unsigned long timePresent = micros();                 // Obtain current time
  unsigned long timeInterval = timePresent - timePast;  // Calculate the elapsed time
  timePast = timePresent;                               // Store the current time as the previous time for the next loop

  // Subtract the timer by the elapsed time between subsequent loop. If the
  // timer value reaches zero, we do the measurement. Then, the timer value will
  // be added by the sampling time period. Thus, the measurement will be
  // conducted in a periodic manner.
  static long timer = 0;
  timer -= timeInterval;
  if (timer < 0) {
    timer += 1000000 / SAMPLING_RATE;
    double t = micros() / 1.0e6;

    // Here we use a signal (f = 3 Hz) imbued with noise (f = 50 Hz)
    double inputSignal = sin(2 * PI * SIGNAL_FREQUENCY * t) + 0.5 * sin(2 * PI * NOISE_FREQUENCY * t);

    // We use lowpass filter to filter out the higher frequency signal
    double filteredSignal = LowpassFilter(inputSignal);

    // Send the results via serial
    Serial.print(inputSignal);
    Serial.print(",");
    Serial.println(filteredSignal);
  }
}

double LowpassFilter(double x_n) {
  /*
    Lowpass Filter, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 500 Hz, Fc = 25 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  // The filter order affects the size of filter coefficients arrays and
  // also used for indexing.
  const int FILTER_ORDER = 4;
  // We made these following variables static so that the stored values carry
  // into subsequent function calls.
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.0004, 0.0017, 0.0025, 0.0017, 0.0004 };
  double a[FILTER_ORDER + 1] = { 1.0000, -3.1806, 3.8612, -2.1122, 0.4383 };

  x[0] = x_n;

  for (int i = 0; i < FILTER_ORDER + 1; i++) {  // Calculate the output signal
    if (i == 0) {
      y[0] = b[i] * x[i];
    } else {
      y[0] += b[i] * x[i] - a[i] * y[i];
    }
  }

  for (int i = FILTER_ORDER; i > 0; i--) {  // Shift elements by one toward the oldest
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}