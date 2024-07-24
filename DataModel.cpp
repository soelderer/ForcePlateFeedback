// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./DataModel.h"

// ____________________________________________________________________________
BalanceParameters::BalanceParameters() {
  // Nothing to do.
  isValid_ = false;
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
void BalanceParameters::validateData() { isValid_ = true; }

// ____________________________________________________________________________
void BalanceParameters::preprocess() { data_ = rawData_; }

// ____________________________________________________________________________
void BalanceParameters::calculateParameters() {
  calculateMeanForceX();
  calculateMeanForceY();
  calculateSwayVariabilityX();
  calculateSwayVariabilityY();
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
DataModel::DataModel() { running_ = false; }

// ____________________________________________________________________________
DataModel::~DataModel() {}

// ____________________________________________________________________________
void DataModel::startProcessing(std::string fileName, float timeframe) {
  fileName_ = fileName;
  timeframe_ = timeframe;

  running_ = true;

  // set up the timer
}

// ____________________________________________________________________________
void DataModel::stopProcessing() {}

// ____________________________________________________________________________
void DataModel::process() {

  kistlerFile_ = KistlerCSVFile(fileName_);

  std::unordered_map<std::string, std::vector<float>> data;
  data = kistlerFile_.getData(0, 5);
}