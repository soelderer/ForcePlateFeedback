// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#pragma once

#include "./KistlerFile.h"
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QTimer>

// The current implementation is not for real live view, but playback of a CSV
// file. This sets the speed of the playback (delay between re-processing in
// ms).
// Benchmarks on my machine indicate that DataModel::process() takes around
// 1-7ms, so sth. like 10ms seems reasonable.
#define PLAYBACK_DELAY_MS 10

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
  BalanceParameters(
      std::shared_ptr<std::unordered_map<std::string, std::vector<float>>>
          data);

  // Default constructor.
  BalanceParameters();

  // Default destructor is enough because we only hold shared_ptr

  // Re-calculate parameters with given data.
  void update(
      const std::shared_ptr<std::unordered_map<std::string, std::vector<float>>>
          &data);

  // Some sanity checks on the provided data.
  void validateData();

  // Pre-process the currently stored data (digital filtering).
  void preprocess();

  // Functions to calculate balance parameters from the pre-processed data.
  void calculateParameters();
  void calculateMeanForceX();
  void calculateMeanForceY();
  // ...

  // Getters.
  float getTimeframe() const { return timeframe_; }
  float getStartTime() const { return startTime_; }
  float getStopTime() const { return stopTime_; }
  float getMeanForceX() const { return meanForceX_; }
  float getMeanForceY() const { return meanForceY_; }
  // ...

private:
  // The raw data.
  std::shared_ptr<std::unordered_map<std::string, std::vector<float>>> rawData_;
  // The preprocessed data.
  std::shared_ptr<std::unordered_map<std::string, std::vector<float>>> data_;

  // If the data (and thus the whole object) is valid.
  bool isValid_;

  // Time information
  float timeframe_;
  float startTime_;
  float stopTime_;
  int numRows_;

  // The parameters.
  float meanForceX_;
  float meanForceY_;
  // more parameters ...

  FRIEND_TEST(BalanceParametersTest, calculateMeanForceX);
  FRIEND_TEST(BalanceParametersTest, calculateMeanForceY);
};

// A class for the data management. It is the "model" in the
// model-view-controller framework. It continously reads data
// and recalculates the balance parameters.
class DataModel : public QObject {
  Q_OBJECT

public:
  DataModel();
  // Qt objects are not supposed to be copied, so no copy constructor and
  // assignment operator implemented. See https://stackoverflow.com/a/19092698

  // Default destructor is enough because we only hold STL and custom classes

private:
  // State variables.
  bool running_;

  // A KistlerFile to read the data from.
  KistlerCSVFile kistlerFile_;

  // Balance parameters, regularly updated by the WorkerThread.
  BalanceParameters balanceParameters_;

  // Name of the data file.
  std::string fileName_;

  // Timeframe in seconds over which the parameters should be calculated
  // (user config).
  float configTimeframe_;

  // Timeframe in seconds over which the current parameters are calculated.
  float timeframe_;
  // Start and stop times of the currently processed timeframe in seconds.
  float startTime_;
  float stopTime_;

  // Number of rows over which the current parameters are calculated.
  int numRows_;
  // First and last row numbers of the currently processed timeframe.
  int firstRow_;
  int lastRow_;

  // Timer for regular re-calculation with newest data.
  QTimer processingTimer_;

private slots:
  // Re-read the latest data and calculate the BalanceParameters.
  // Emits a signal when parameters a ready for display.
  void process();

public slots:
  void onStartProcessing(std::string fileName, float timeframe);
  void onStopProcessing();
  void onResetModel();

signals:
  void dataUpdated(BalanceParameters *balanceParameters);
  void reachedEOF();
  void invalidFileSignal(std::string fileName_);
  void corruptFileSignal();
};