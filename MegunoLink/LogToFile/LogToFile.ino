/* ***********************************************************************
Example program to send CSV data through Megunolinks message system and
log data to a text file based on a file name generated by this program.

The MegunoLink project for this example is in the same folder as this
Arduino code. To download a free MegunoLink trial, visit:
  - http://www.MegunoLink.com to download MegunoLink.

More Information
  - https://www.megunolink.com/documentation/logging/message-logger/
  - https://www.megunolink.com/documentation/getting-started/processing-serial-commands/
  - https://www.megunolink.com/documentation/arduino-libraries/arduino-timer/

This example requires:
  - Our MegunoLink Arduino library
    https://www.megunolink.com/documentation/arduino-library/
* *********************************************************************** */

#include "MegunoLink.h"
#include "CommandHandler.h"
#include "ArduinoTimer.h"

const unsigned long kSamplingRateHz = 50;
const unsigned long kBaudRate = 115200;
const double kSignalFreqHz = 1;

Message Msg;
CommandHandler<> Cmds;
::ArduinoTimer ExperimentTimer;


int FileNumber = 1;
int MeasurementsSaved = 1;
const int MeasurementsPerFile = 50;
const int NumberOfRuns = 1;

bool ExperimentRunning;

void StartNewLogFile(int LogFileNumber) {
  String FileName("Log");
  FileName += LogFileNumber;
  FileName += ".txt";
  Msg.LogTo(FileName);
  MeasurementsSaved = 0;
}

void StoreMeasurement() {
  double t = micros() / 1.0e6;
  auto Value = sin(2 * PI * kSignalFreqHz * t);
  // auto Value = analogRead(0);
  Msg.Send(millis(), Value);
  ++MeasurementsSaved;
}

void Cmd_StartExperiment(CommandParameter &p) {
  ExperimentRunning = true;
  FileNumber = 0;
  StartNewLogFile(++FileNumber);
}

void Cmd_StopExperiment(CommandParameter &p) {
  Msg.StopLogging();
  ExperimentRunning = false;
}

void setup() {
  Serial.begin(9600);
  Cmds.AddCommand(F("Start"), Cmd_StartExperiment);
  Cmds.AddCommand(F("Stop"), Cmd_StopExperiment);
}

void loop() {
  Cmds.Process();

  if (ExperimentRunning) {
      static unsigned long past_time_us = 0;
      unsigned long present_time_us = micros();
      unsigned long time_interval_us = present_time_us - past_time_us;
      past_time_us = present_time_us;

      static double timer_us = 0;
    timer_us -= time_interval_us;
    if (timer_us < 0) {
      timer_us += 1e6 / kSamplingRateHz;
    // if (ExperimentTimer.TimePassed_Milliseconds(250)) {
      StoreMeasurement();
    }

    if (MeasurementsSaved == MeasurementsPerFile) {
      ++FileNumber;
      if (FileNumber <= NumberOfRuns) {
        // Start a new experiment run.
        StartNewLogFile(FileNumber);
      } else {
        // Stop the experiment when all runs have been completed
        ExperimentRunning = false;
        Msg.StopLogging();
      }
    }
  }
}
