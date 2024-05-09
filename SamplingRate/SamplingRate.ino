/*
  Setting Sampling Rate
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

const unsigned long SAMPLING_RATE = 500;  // If this value changed, filter coefficients of all filter functions must be recalculated
const unsigned long BAUD_RATE = 115200;
const float SIGNAL_FREQUENCY = 3;

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  // Calculate elapsed time
  static unsigned long timePast = 0;                    // Initiate static variable to store previous time
  unsigned long timePresent = micros();                 // Obtain current time
  unsigned long timeInterval = timePresent - timePast;  // Calculate the elapsed time
  timePast = timePresent;                               // Store the current time as the previous time for the next loop

  // Run the timer
  // Subtract the timer by the elapsed time between subsequent loop. If the
  // timer value reaches zero, we do the measurement. Then, the timer value will
  // be added by the sampling time period. Thus, the measurement will be
  // conducted in a periodic manner.
  static long timer = 0;
  timer -= timeInterval;
  if (timer < 0) {
    timer += 1000000 / SAMPLING_RATE;
    double t = micros() / 1.0e6;

    // Here we use a signal (f = 3 Hz)
    double inputSignal = sin(2 * PI * SIGNAL_FREQUENCY * t);

    // Send the results via serial
    Serial.println(inputSignal);
  }
}