// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#pragma once

#include "./KistlerFile.h"
#include <QtCore/QObject>

// Placeholder for biomechanical toolkit (BTK):
// https://biomechanical-toolkit.github.io/docs/API/

// Class for the balance parameters for a specified timeframe (e.g. 50ms).
// The constructor takes a KistlerDatFile as input. There are methods for
// re-calculating the parameters (i.e. to regularly update the parameters
// with the latest data).
// The balance parameters are calculated from the raw data (i.e. forces,
// moments, force application point). The specific parameters are to be
// determined in the course of the project (it will need some trial and error
// to see what makes sense for the participants, and depends on theoretical
// assumptions). Some options are mean forces in X and Y direction, or more
// complicated parameters like sway variability.
class BalanceParameters {
public:
  // Constructor with specified start and stop times. This will determine the
  // timeframe.
  BalanceParameters(KistlerFile *kistlerFile, float startTime, float stopTime) {
  }
  // Constructor with specified timeframe. This will set stopTime to the latest
  // available data point and determine startTime. Useful to retrieve e.g.
  // the latest 50 ms of data.
  BalanceParameters(KistlerCSVFile *kistlerFile, float timeframe) {}
  BalanceParameters() {}
  ~BalanceParameters() {}

  // Update the data, i.e. get and store the latest data from the file while
  // keeping the timeframe constant.
  void updateData() {}

  // Pre-process the currently stored data (digital filtering).
  void preprocess() {}

  // Re-calculate the parameters with the current precessed data.
  void recalculate() {}

  // Functions to calculate balance parameters from the pre-processed data.
  void calculateMeanForceX() {}
  void calculateMeanForceY() {}
  void calculateSwayVariabilityX() {}
  void calculateSwayVariabilityY() {}
  // ...

  // Getters.
  float getTimeframe() const { return 1.0; }
  float getStartTime() const { return 1.0; }
  float getStopTime() const { return 1.0; }
  float getMeanForceX() const { return 1.0; }
  float getMeanForceY() const { return 1.0; }
  float getSwayVariabilityX() const { return 1.0; }
  float getSwayVariabilityY() const { return 1.0; }
  // ...

private:
  // A KistlerFile to read the data from.
  KistlerFile *KisterDatFile_;
  // Timeframe in seconds over which the parameters are calculated.
  float timeframe_;
  // Start and stop times of the timeframe in seconds.
  float startTime_;
  float stopTime_;
  // Maybe it's necessary to implemement start and stop based on row indices as
  // well.

  // The raw data.
  std::unordered_map<std::string, std::vector<float>> rawData_;
  // The preprocessed data.
  std::unordered_map<std::string, std::vector<float>> data_;

  // The parameters.
  float meanForceX_;
  float meanForceY_;
  float swayVariabilityX_;
  float swayVariabilityY_;
  // more parameters ...

  FRIEND_TEST(BalanceParametersTest, calculateMeanForceX);
};

// A class for the data management. It is the "model" in the
// model-view-controller framework. It continously reads data
// and recalculates the balance parameters.
class DataModel : public QObject {
  Q_OBJECT

private:
  KistlerCSVFile kistlerFile_;
  BalanceParameters balanceParameters_;
};