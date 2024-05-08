/*
  Analytic Envelope Using Hilbert Transformer
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLE_RATE = 500;  // If this value changed, filter coefficients of all filter functions must be recalculated
const unsigned long BAUD_RATE = 115200;
const float MODULATION_FREQUENCY = 3;
const float CARRIER_FREQUENCY = 50;

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

    // Here we use amplitude modulation (AM) as our input signal. The carrier
    // signal amplitude is varied in accordance with the instantaneous amplitude
    // of the modulation signal. First, the modulation signal amplitude is
    // shifted by one. Then, its instantaneous amplitude value is multiplied
    // with the instantaneous amplitude of the carrier signal.
    double modulationSignal = sin(2 * PI * MODULATION_FREQUENCY * t);  // Modulation signal (f = 3 Hz)
    double carrierSignal = sin(2 * PI * CARRIER_FREQUENCY * t);        // Carrier signal (f = 50 Hz)
    double signalAM = (1 + modulationSignal) * carrierSignal;
    Serial.print(signalAM);

    double outputHilbert = HilbertTransformer(signalAM);
    double outputCenterTap = CenterTap(signalAM);
    double envelope = sqrt(outputCenterTap * outputCenterTap + outputHilbert * outputHilbert);

    Serial.print(",");
    Serial.print(outputHilbert);
    Serial.print(",");
    Serial.print(outputCenterTap);
    Serial.print(",");
    Serial.println(envelope);
  }
}

double HilbertTransformer(double x_n) {
  /*
    This filter is a noncausal filter. Thus, the input signal must be delayed by
    half of the filter order (the middle coefficient corresponds to the current
    input signal value).
  */
  double b[19] = { -4.0 / 1024, 0.0, -21.0 / 1024, 0, -64.0 / 1024, 0, -170.0 / 1024, 0, -634.0 / 1024, 0, 634.0 / 1024, 0, 170.0 / 1024, 0, 64.0 / 1024, 0, 21.0 / 1024, 0, 4.0 / 1024 };
  static double x_n1, x_n2, x_n3, x_n4, x_n5, x_n6, x_n7, x_n8, x_n9, x_n10, x_n11, x_n12, x_n13, x_n14, x_n15, x_n16, x_n17, x_n18, x_n19;
  double y_n = b[0] * x_n + b[1] * x_n1 + b[2] * x_n2 + b[3] * x_n3 + b[4] * x_n4 + b[5] * x_n5 + b[6] * x_n6 + b[7] * x_n7 + b[8] * x_n8 + b[9] * x_n9 + b[10] * x_n10 + b[11] * x_n11 + b[12] * x_n12 + b[13] * x_n13 + b[14] * x_n14 + b[15] * x_n15 + b[16] * x_n16 + b[17] * x_n17 + b[18] * x_n18 + b[19] * x_n19;

  x_n19 = x_n18;
  x_n18 = x_n17;
  x_n17 = x_n16;
  x_n16 = x_n15;
  x_n15 = x_n14;
  x_n14 = x_n13;
  x_n13 = x_n12;
  x_n12 = x_n11;
  x_n11 = x_n10;
  x_n10 = x_n9;
  x_n9 = x_n8;
  x_n8 = x_n7;
  x_n7 = x_n6;
  x_n6 = x_n5;
  x_n5 = x_n4;
  x_n4 = x_n3;
  x_n3 = x_n2;
  x_n2 = x_n1;
  x_n1 = x_n;

  return y_n;
}

double DelayFilter(double x_n) {
  /*
    Delay filter for delaying the input signal to accomodate the noncausal
    nature of the Hilbert Transformer.
  */
  double b[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  static double x_n1, x_n2, x_n3, x_n4, x_n5, x_n6, x_n7, x_n8, x_n9, x_n10, x_n11, x_n12, x_n13, x_n14, x_n15, x_n16, x_n17, x_n18, x_n19;
  double y_n = b[0] * x_n + b[1] * x_n1 + b[2] * x_n2 + b[3] * x_n3 + b[4] * x_n4 + b[5] * x_n5 + b[6] * x_n6 + b[7] * x_n7 + b[8] * x_n8 + b[9] * x_n9 + b[10] * x_n10 + b[11] * x_n11 + b[12] * x_n12 + b[13] * x_n13 + b[14] * x_n14 + b[15] * x_n15 + b[16] * x_n16 + b[17] * x_n17 + b[18] * x_n18 + b[19] * x_n19;

  x_n19 = x_n18;
  x_n18 = x_n17;
  x_n17 = x_n16;
  x_n16 = x_n15;
  x_n15 = x_n14;
  x_n14 = x_n13;
  x_n13 = x_n12;
  x_n12 = x_n11;
  x_n11 = x_n10;
  x_n10 = x_n9;
  x_n9 = x_n8;
  x_n8 = x_n7;
  x_n7 = x_n6;
  x_n6 = x_n5;
  x_n5 = x_n4;
  x_n4 = x_n3;
  x_n3 = x_n2;
  x_n2 = x_n1;
  x_n1 = x_n;

  return y_n;
}