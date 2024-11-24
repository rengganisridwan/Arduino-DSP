/*
  Onset Detection with Digital Push Button
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

#include <MegunoLink.h>
#include <CommandHandler.h>

TimePlot MyPlot;
CommandHandler<> SerialCommandHandler;

const unsigned long kSamplingRate = 300;
const unsigned long kBaudRate = 500000;
static bool is_button_pressed = false;

void setup() {
  Serial.begin(kBaudRate);

  pinMode(LED_BUILTIN, OUTPUT);
  SerialCommandHandler.AddCommand(F("ButtonSwitch"), CmdButtonSwitch);
}

void loop() {
  SerialCommandHandler.Process();
  
  static unsigned long past_time_us = 0;
  unsigned long present_time_us = micros();
  unsigned long time_interval_us = present_time_us - past_time_us;
  past_time_us = present_time_us;

  static double timer_us = 0;
  timer_us -= time_interval_us;

  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRate;
    double t = micros() / 1.0e6;
    double t_ms = millis();

    double raw_signal = sin(2*PI*30*t);
    // double raw_signal = analogRead(A7);
    double filtered_signal = BandpassFilter(raw_signal);
    filtered_signal = BandstopFilter50(filtered_signal);
    filtered_signal = BandstopFilter100(filtered_signal);

    double abs_signal = abs(filtered_signal);
    double lp_envelope = LowpassFilter(abs_signal);

    // MyPlot.SendData("time (ms)",t_ms);
    // MyPlot.SendData("raw",raw_signal);
    // MyPlot.SendData("filtered",filtered_signal);
    // MyPlot.SendData("envelope",lp_envelope);
  }
}

bool CmdButtonSwitch() {
  if (is_button_pressed == false) {
    digitalWrite(LED_BUILTIN, HIGH);
    return is_button_pressed = true;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    return is_button_pressed = false;
  }
}

void TurnOffLED() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("fungsi turn off led jalan");
}

double CmdSetEnvelopeTreshold(double raw_data) {
  // unsigned int kBufferSize = 300;
  // double buffer_x1[kBufferSize];
  // double buffer_x2[kBufferSize]; 
  static double n, var, mean, mean_prev, stdev;
  if (is_button_pressed == 0) {
    is_button_pressed = 1;
    digitalWrite(LED_BUILTIN, HIGH);

    n += 1;
    mean_prev = mean;
    mean = mean + (raw_data - mean) / n;
    var = var + (raw_data - mean) * (raw_data - mean_prev);
    stdev = sqrt(var/n);
  }
  return stdev;
}

double BandstopFilter50(double x_n) {
  /*
    50 Hz Bandstop (Notch) Filter, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 300 Hz, Fc1 = 48 Hz, Fc2 = 52 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  const int FILTER_ORDER = 4;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.9425,   -1.8866,    2.8291,   -1.8866,    0.9425 };
  double a[FILTER_ORDER + 1] = { 1.0000,   -1.9425,    2.8258,   -1.8307,    0.8883 };

  x[0] = x_n;
  y[0] = b[0] * x[0];

  for (int i = 1; i < FILTER_ORDER + 1; i++) {
    y[0] += b[i] * x[i] - a[i] * y[i];
  }

  for (int i = FILTER_ORDER; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}

double BandstopFilter100(double x_n) {
  /*
    50 Hz Bandstop (Notch) Filter, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 300 Hz, Fc1 = 98 Hz, Fc2 = 102 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  const int FILTER_ORDER = 4;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.9425,    1.8866,    2.8291,    1.8866,    0.9425 };
  double a[FILTER_ORDER + 1] = { 1.0000,    1.9425,    2.8258,    1.8307,    0.8883 };

  x[0] = x_n;
  y[0] = b[0] * x[0];

  for (int i = 1; i < FILTER_ORDER + 1; i++) {
    y[0] += b[i] * x[i] - a[i] * y[i];
  }

  for (int i = FILTER_ORDER; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}

double HighpassFilter(double x_n) {
  /*
    Bandpass, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 300 Hz, Fc = 10 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  // We made these following variables static so that the stored values carry
  // into subsequent function calls.

  const int FILTER_ORDER = 4;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.7602,   -3.0406,    4.5609,   -3.0406,    0.7602 };
  double a[FILTER_ORDER + 1] = { 1.0000,   -3.4532,    4.5041,   -2.6273,    0.5778 };

  x[0] = x_n;
  y[0] = b[0] * x[0];

  for (int i = 1; i < FILTER_ORDER + 1; i++) {
    y[0] += b[i] * x[i] - a[i] * y[i];
  }

  for (int i = FILTER_ORDER; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}

double BandpassFilter(double x_n) {
  /*
    Bandpass, Butterworth IIR, 4th Order
    Direct-Form II, Single Section
    Fs = 300 Hz, Fc1 = 10 Hz, Fc2 = 148 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  // We made these following variables static so that the stored values carry
  // into subsequent function calls.

  const int FILTER_ORDER = 4;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.8371,         0,   -1.6742,         0,    0.8371 };
  double a[FILTER_ORDER + 1] = { 1.0000,    0.2354,   -1.6236,   -0.1640,    0.7009 };

  x[0] = x_n;
  y[0] = b[0] * x[0];

  for (int i = 1; i < FILTER_ORDER + 1; i++) {
    y[0] += b[i] * x[i] - a[i] * y[i];
  }

  for (int i = FILTER_ORDER; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}

double LowpassFilter(double x_n) {
  /*
    Lowpass Filter, Butterworth IIR, 2th Order
    Direct-Form II, Single Section
    Fs = 500 Hz, Fc = 6 Hz
    The filter coefficients are obtained from MATLAB filterDesigner
  */

  const int FILTER_ORDER = 2;
  static double x[FILTER_ORDER + 1] = {};
  static double y[FILTER_ORDER + 1] = {};
  double b[FILTER_ORDER + 1] = { 0.00134871194835634, 0.00269742389671269, 0.00134871194835634 };
  double a[FILTER_ORDER + 1] = { 1, -1.89346414636183, 0.898858994155252 };

  x[0] = x_n;

  for (int i = 0; i < FILTER_ORDER + 1; i++) {  // Calculate the output signal
    if (i == 0) {
      y[0] = b[i]*x[i];
    } else {
      y[0] += b[i]*x[i] - a[i]*y[i];
    }
  }

  for (int i = FILTER_ORDER; i > 0; i--) {  // Shift elements by one toward the oldest
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }

  return y[0];
}