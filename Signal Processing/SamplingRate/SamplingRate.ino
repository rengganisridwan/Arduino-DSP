/*
  Setting Sampling Rate
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLING_RATE_HZ = 500;  // If this value changed, filter coefficients of all filter functions must be recalculated
const unsigned long BAUD_RATE = 115200;
const double SIGNAL_FREQUENCY_HZ = 3;
const double NOISE_FREQUENCY_HZ = 50;

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  // Calculate elapsed time
  static unsigned long pastTimeMicrosecond = 0;                                          // Initiate static variable to store previous time
  unsigned long presentTimeMicrosecond = micros();                                       // Obtain current time
  unsigned long timeIntervalMicrosecond = presentTimeMicrosecond - pastTimeMicrosecond;  // Calculate the elapsed time
  pastTimeMicrosecond = presentTimeMicrosecond;                                          // Store the current time as the previous time for the next loop

  // Subtract the timer by the elapsed time between subsequent loop. If the
  // timer value reaches zero, we do the measurement. Then, the timer value will
  // be added by the sampling time period. Thus, the measurement will be
  // conducted in a periodic manner.
  static long timerMicrosecond = 0;
  timerMicrosecond -= timeIntervalMicrosecond;
  if (timerMicrosecond < 0) {
    timerMicrosecond += 1000000 / SAMPLING_RATE_HZ;
    double t = micros() / 1.0e6;

    double inputSignal = sin(2 * PI * SIGNAL_FREQUENCY_HZ * t);
    double noiseSignal = 0.5 * sin(2 * PI * NOISE_FREQUENCY_HZ * t);
    double combinedSignal = inputSignal + noiseSignal;
    Serial.println(combinedSignal);
  }
}