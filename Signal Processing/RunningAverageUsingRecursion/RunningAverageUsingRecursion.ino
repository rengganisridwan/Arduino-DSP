/*
  Calculate Running Average Using Recursion
  https://github.com/rengganisridwan/Arduino-DSP

  Author: Rengganis R.H. Santoso
*/

#include <MegunoLink.h>
TimePlot MyPlot;

const unsigned long kSamplingRate = 300;
const unsigned long kBaudRate = 500000;

const int N = 100;  // Window size for running average
// int circular_buffer[kBufferSize];

void setup() {
  Serial.begin(kBaudRate);
  // for (int i = 0; i < RA_kFilterOrder + 1; i++) {
  //   RA_b[i] = 1;
  // }
}

void loop() {
  static unsigned long t_past_us = 0;
  unsigned long t_present_us = micros();
  unsigned long t_interval_us = t_present_us - t_past_us;
  t_past_us = t_present_us;

  static double timer_us = 0;
  timer_us -= t_interval_us;
  if (timer_us < 0) {
    timer_us += 1e6 / kSamplingRate;

    double t = micros() / 1.0e6;
    int raw_signal = 20*sin(2 * PI * 1 * t);
    int mean = RunningAverage(raw_signal);
    int meansq = RunningAverageSq(raw_signal);

    int var = meansq - mean*mean;
    int stdev = sqrt(var); 

    MyPlot.SendData("raw",raw_signal);
    MyPlot.SendData("mean",3*stdev);
  }
}

int RunningAverage(int x_n)  {
  /*
    Running Average Filter Using Recursion    
  */

  const int kFilterOrder = N;
  static double x[kFilterOrder + 1] = {};
  static double y[2] = {};

  x[0] = x_n;
  y[0] = x[0]/N + y[1] - x[N]/N;

  y[1] = y[0];
  for (int i = kFilterOrder; i > 0; i--) {
    x[i] = x[i - 1];
  }

  return y[0];
}

int RunningAverageSq(int x_n)  {
  /*
    Running Average Filter Using Recursion 

    Note:
      To make this function more efficient, reduce the array size for x.
      We only need the first and the last value of x:
      x[n] and x[n-N]
  */
  
  x_n = x_n*x_n;
  const int kFilterOrder = N;
  static double x[kFilterOrder + 1] = {};
  static double y[2] = {};

  x[0] = x_n;
  y[0] = x[0]/N + y[1] - x[N]/N;

  y[1] = y[0];
  for (int i = kFilterOrder; i > 0; i--) {
    x[i] = x[i - 1];
  }

  return y[0];
}

// int RunningAverage(int x_n){
//   /*
//     Running Average Using Circular Buffer
//     Global variables takes 601 bytes of dynamic memory
//   */

//   static int idx, sum;

// 	sum -= circular_buffer[idx]; // This line delete the oldest value in buffer
// 	sum += x_n;
// 	circular_buffer[idx] = x_n; // save it into array 
// 	idx = (idx + 1) % kBufferSize;

// 	return (sum/kBufferSize);
// }