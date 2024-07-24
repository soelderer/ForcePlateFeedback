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
  // Constructor with data provided.
  BalanceParameters(std::unordered_map<std::string, std::vector<float>> *data);

  // Default constructor.
  BalanceParameters();
  ~BalanceParameters() {}

  // Re-calculate parameters with given data.
  void update(std::unordered_map<std::string, std::vector<float>> *data) {}

  // Some sanity checks on the provided data.
  void validateData();

  // Pre-process the currently stored data (digital filtering).
  void preprocess() {}

  // Functions to calculate balance parameters from the pre-processed data.
  void calculateParameters();
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
  // The raw data.
  std::unordered_map<std::string, std::vector<float>> *rawData_;
  // The preprocessed data.
  std::unordered_map<std::string, std::vector<float>> data_;

  // If the data (and thus the whole object) is valid.
  bool isValid_;

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

public:
  DataModel();
  ~DataModel();

private:
  // State variables.
  bool running_;

  // A KistlerFile to read the data from.
  KistlerCSVFile kistlerFile_;

  // Balance parameters, regularly updated by the DataModel.
  BalanceParameters balanceParameters_;

  // Name of the data file.
  std::string fileName_;

  // Timeframe in seconds over which the parameters are calculated.
  float timeframe_;
  // Start and stop times of the timeframe in seconds.
  float startTime_;
  float stopTime_;

public slots:
  void onStartLiveView(std::string fileName, float timeframe);
  void onStopLiveView();
};