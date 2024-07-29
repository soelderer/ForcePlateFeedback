// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./DataModel.h"

// ____________________________________________________________________________
BalanceParameters::BalanceParameters() {
  // Nothing to do.
  isValid_ = false;

  timeframe_ = 0;
  startTime_ = 0;
  stopTime_ = 0;
  numRows_ = 0;
}

// ____________________________________________________________________________
BalanceParameters::BalanceParameters(
    std::unordered_map<std::string, std::vector<float>> *data) {
  rawData_ = data;

  validateData();
  preprocess();
  calculateParameters();
}

// ____________________________________________________________________________
void BalanceParameters::update(
    std::unordered_map<std::string, std::vector<float>> *data) {
  rawData_ = data;

  validateData();
  preprocess();
  calculateParameters();
}

// ____________________________________________________________________________
void BalanceParameters::validateData() {
  // Data is empty.
  if (rawData_->size() == 0) {
    timeframe_ = 0;
    startTime_ = 0;
    stopTime_ = 0;
    numRows_ = 0;

    isValid_ = false;
    return;
  }

  // Check if there are at least the columns for time and force in x and y
  // direction (add more checks if other parameters are calculated).
  if (rawData_->count("abs time (s)") == 0 || rawData_->count("Fx") == 0 ||
      rawData_->count("Fy") == 0) {
    timeframe_ = 0;
    startTime_ = 0;
    stopTime_ = 0;
    numRows_ = 0;

    isValid_ = false;
    return;
  }

  numRows_ = (*rawData_)["abs time (s)"].size();
  startTime_ = (*rawData_)["abs time (s)"].front();
  stopTime_ = (*rawData_)["abs time (s)"].back();
  timeframe_ = stopTime_ - startTime_;

  // TODO: real sanity checks ...

  isValid_ = true;
}

// ____________________________________________________________________________
void BalanceParameters::preprocess() { data_ = rawData_; }

// ____________________________________________________________________________
void BalanceParameters::calculateParameters() {
  calculateMeanForceX();
  calculateMeanForceY();
  // ...
}

// ____________________________________________________________________________
void BalanceParameters::calculateMeanForceX() {
  if ((*data_)["Fx"].size() == 0) {
    meanForceX_ = 0;
    return;
  }

  meanForceX_ = std::reduce((*data_)["Fx"].begin(), (*data_)["Fx"].end()) /
                static_cast<float>((*data_)["Fx"].size());
}

// ____________________________________________________________________________
void BalanceParameters::calculateMeanForceY() {
  if ((*data_)["Fy"].size() == 0) {
    meanForceY_ = 0;
    return;
  }

  meanForceY_ = std::reduce((*data_)["Fy"].begin(), (*data_)["Fy"].end()) /
                static_cast<float>((*data_)["Fy"].size());
}

// ____________________________________________________________________________
DataModel::DataModel() : running_(false) {
  fileName_ = "";

  configTimeframe_ = 0;

  startTime_ = 0;
  stopTime_ = 0;
  timeframe_ = 0;

  firstRow_ = 0;
  lastRow_ = 0;
  numRows_ = 0;

  // Set up a timer for regular reprocessing.
  // Current implementation is for playback of pre-existing CSV files,
  // in a later stage we will switch to live view -> timers need to be
  // adjusted.
  processingTimer_.setInterval(PLAYBACK_DELAY_MS);

  QObject::connect(&processingTimer_, &QTimer::timeout, this,
                   &DataModel::process);
}

// ____________________________________________________________________________
DataModel::~DataModel() {}

// ____________________________________________________________________________
void DataModel::onStartProcessing(std::string fileName, float timeframe) {
  configTimeframe_ = timeframe;

  // New file configured.
  if (fileName != fileName_) {
    fileName_ = fileName;
    kistlerFile_ = KistlerCSVFile(fileName_);
  }

  running_ = true;

  if (!processingTimer_.isActive())
    processingTimer_.start();
}

// ____________________________________________________________________________
void DataModel::onStopProcessing() {
  if (processingTimer_.isActive()) {
    processingTimer_.stop();
    running_ = false;
  }
}

// ____________________________________________________________________________
void DataModel::process() {
  // Determine number of rows we need to read with sampling rate and the
  // configured timeframe.
  // (sampling rate is guaranteed to be != 0)

  int attemptedNumRows = configTimeframe_ * kistlerFile_.getSamplingRate();

  std::unordered_map<std::string, std::vector<float>> data;

  data = kistlerFile_.getData(firstRow_, firstRow_ + attemptedNumRows - 1);

  if (data["abs time (s)"].size() != 0) {
    balanceParameters_.update(&data);

    // Period is 1 / sampling rate, * 1000 to get it in miliseconds.
    firstRow_ =
        firstRow_ + PLAYBACK_DELAY_MS / kistlerFile_.getSamplingRate() * 1000;

    lastRow_ = firstRow_ + data["abs time (s)"].size();
    numRows_ = data["abs time (s)"].size();

    startTime_ = balanceParameters_.getStartTime();
    stopTime_ = balanceParameters_.getStopTime();
    timeframe_ = balanceParameters_.getTimeframe();
  }

  emit dataUpdated(&balanceParameters_);

  // Check if we reached EOF.
  if (data["abs time (s)"].size() < attemptedNumRows) {
    qDebug() << "DataModel::process(): reached EOF";
    emit reachedEOF();
  }
}

// ____________________________________________________________________________
void DataModel::onResetModel() {
  onStopProcessing();

  startTime_ = 0;
  stopTime_ = 0;
  timeframe_ = 0;

  firstRow_ = 0;
  lastRow_ = 0;
  numRows_ = 0;

  // balanceParameters_ = new BalanceParameters();
}