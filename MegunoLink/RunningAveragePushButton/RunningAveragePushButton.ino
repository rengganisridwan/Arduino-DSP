/*
  Calculate Running Average Using Recursion
  for Limited Sample Stream N
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

#include <MegunoLink.h>
TimePlot MyPlot;
#include <CommandHandler.h>
CommandHandler<> SerialCommandHandler;

const unsigned long kSamplingRate = 300;
const unsigned long kBaudRate = 500000;

bool is_button_pressed = false;
int mean_idx = 0;
const int N = 300;  // Window size for running average

void setup() {
  Serial.begin(kBaudRate);

  pinMode(LED_BUILTIN, OUTPUT);
  SerialCommandHandler.AddCommand(F("ButtonSwitch"), CmdButtonSwitch);
}

void loop() {
  SerialCommandHandler.Process();
  static int mean = 0;

  static unsigned long t_past_us = 0;
  unsigned long t_present_us = micros();
  unsigned long t_interval_us = t_present_us - t_past_us;
  t_past_us = t_present_us;

  static double timer_us = 0;
  timer_us -= t_interval_us;
  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRate;
    
    double t = micros() / 1.0e6;
    int raw_signal = 200*sin(2 * PI * 1 * t);

    mean = RunningAverage(raw_signal);

    MyPlot.SendData("raw",mean_idx);
    MyPlot.SendData("mean",mean);

    if (mean_idx < N) {
      mean_idx += 1;
    }
    if (mean_idx == N) {
      digitalWrite(LED_BUILTIN, LOW);
      is_button_pressed = false;
    }
  }
}

int RunningAverage(int x_n)  {
  /*
    Running Average Filter Using Recursion    
  */

  // static int idx = 0;
  // const int kFilterOrder = N;
  static double x[1] = {};
  static double y[2] = {};

  if (mean_idx == 0) {
    x[1] = 0;
    y[0] = 0;
    y[1] = 0;
  }
  if (mean_idx == N) {
    // CmdButtonSwitch();
    return y[0];
  } 
  if (is_button_pressed == true) {
    x[0] = x_n;
    y[0] = x[0]/N + y[1];

    y[1] = y[0];
    mean_idx += 1;

    return y[0];
  }
}

bool CmdButtonSwitch() {
  if (is_button_pressed == false) {
    mean_idx = 0;
    digitalWrite(LED_BUILTIN, HIGH);
    return is_button_pressed = true;
  }
}