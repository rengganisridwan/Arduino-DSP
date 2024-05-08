/*
  Bandpass Filter
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLE_RATE = 500;  // If this value changed, filter coefficients of all filter functions must be recalculated
const unsigned long BAUD_RATE = 115200;
const float SIGNAL_FREQUENCY = 1;
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

  // Run timer
  // Subtract the timer by the elapsed time between subsequent loop. If the
  // timer value reaches zero, we do the measurement. Then, the timer value will
  // be added by the sampling time period. Thus, the measurement will be
  // conducted in a periodic manner.
  static long timer = 0;
  timer -= timeInterval;
  if (timer < 0) {
    timer += 1000000 / SAMPLE_RATE;
    double t = micros() / 1.0e6;

    // Here we use a signal (f = 3 Hz) imbued with noise (f = 50 Hz)
    double inputSignal = sin(2 * PI * SIGNAL_FREQUENCY * t) + 0.5 * sin(2 * PI * NOISE_FREQUENCY * t);

    // We use bandpass filter to filter out the lower frequency signal
    double outputBandpass = BandpassFilter(inputSignal);

    // Now we use the same specification of the previous bandpass filter
    // but with different structure. The following bandpass filter is composed
    // of second-order sections.
    double outputSOSBandpass1 = SOSBandpassFilter_1(inputSignal);         // Filtering using the 1st section
    double outputSOSBandpass2 = SOSBandpassFilter_2(outputSOSBandpass1);  // Filtering using the 2nd section

    // Send the results via serial
    Serial.print(inputSignal);
    Serial.print(",");
    Serial.print(outputBandpass);
    Serial.print(",");
    Serial.println(outputSOSBandpass2);
  }
}

double BandpassFilter(double x_n) {
  /*
    Bandpass, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 500 Hz, Fc1 = 10 Hz, Fc2 = 200 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  // We made these following variables static so that the stored values carry
  // into subsequent function calls.
  static double x_n1, x_n2, x_n3, x_n4, y_n1, y_n2, y_n3, y_n4;

  double b[5] = { 0.5825, 0, -1.1650, -0.0000, 0.5825 };
  double a[5] = { 1.0000, -0.6874, -0.8157, 0.1939, 0.3477 };
  double inputs = b[0] * x_n + b[1] * x_n1 + b[2] * x_n2 + b[3] * x_n3 + b[4] * x_n4;
  double y_n = inputs - (a[1] * y_n1 + a[2] * y_n2 + a[3] * y_n3 + a[4] * y_n4);

  x_n4 = x_n3;
  x_n3 = x_n2;
  x_n2 = x_n1;
  x_n1 = x_n;

  y_n4 = y_n3;
  y_n3 = y_n2;
  y_n2 = y_n1;
  y_n1 = y_n;

  return y_n;
}

double SOSBandpassFilter_1(double x_n) {
  /*
    Bandpass, Butterworth IIR, 4th Order
    Direct-Form II, Second-Order Sections (First Section)
    Fs = 500 Hz, Fc1 = 10 Hz, Fc2 = 200 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  static double x_n1, x_n2, y_n1, y_n2;
  double b[3] = { 1, 0, -1 };
  double a[3] = { 1, -1.8224, 0.8375 };
  double GAIN = 0.7632;

  double inputs = b[0] * x_n + b[1] * x_n1 + b[2] * x_n2;
  double y_n = GAIN * inputs - (a[1] * y_n1 + a[2] * y_n2);

  x_n2 = x_n1;
  x_n1 = x_n;

  y_n2 = y_n1;
  y_n1 = y_n;

  return y_n;
}

double SOSBandpassFilter_2(double x_n) {
  /*
    Bandpass, Butterworth IIR, 4th Order
    Direct-Form II, Second-Order Sections (Second Section)
    Fs = 500 Hz, Fc1 = 10 Hz, Fc2 = 200 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */
  
  static double x_n1, x_n2, y_n1, y_n2;
  double b[3] = { 1, 0, -1 };
  double a[3] = { 1, 1.1349, 0.4151 };
  double GAIN = 0.7632;

  double inputs = b[0] * x_n + b[1] * x_n1 + b[2] * x_n2;
  double y_n = GAIN * inputs - (a[1] * y_n1 + a[2] * y_n2);

  x_n2 = x_n1;
  x_n1 = x_n;

  y_n2 = y_n1;
  y_n1 = y_n;

  return y_n;
}