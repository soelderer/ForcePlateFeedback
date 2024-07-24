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
void BalanceParameters::calculateParameters() {
  calculateMeanForceX();
  calculateMeanForceY();
  calculateSwayVariabilityX();
  calculateSwayVariabilityY();
}

// ____________________________________________________________________________
DataModel::DataModel() { running_ = false; }

// ____________________________________________________________________________
DataModel::~DataModel() {}

// ____________________________________________________________________________
void DataModel::onStartLiveView(std::string fileName, float timeframe) {
  fileName_ = fileName;
  timeframe_ = timeframe;

  running_ = true;

  kistlerFile_ = KistlerCSVFile(fileName_);

  std::unordered_map<std::string, std::vector<float>> data;
  data = kistlerFile_.getData(100, 103);
}

// ____________________________________________________________________________
void DataModel::onStopLiveView() {}