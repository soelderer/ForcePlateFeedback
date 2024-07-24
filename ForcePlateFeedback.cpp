// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./ForcePlateFeedback.h"

// ____________________________________________________________________________
ConfigWindow::ConfigWindow() {
  window_ = new QWidget();
  window_->setFixedSize(400, 200);

  QGridLayout *windowLayout = new QGridLayout;

  startButton_ = new QPushButton("Start");
  setFileButton_ = new QPushButton("Set data file");
  fileLineEdit_ = new QLineEdit();

  timeLabel_ = new QLabel("Time frame (ms)");
  timeLineEdit_ = new QLineEdit("50");
  timeLineEdit_->setValidator(new QIntValidator(1, MAX_TIMEFRAME, this));

  fileDialog_ = new QFileDialog();

  windowLayout->addWidget(startButton_, 2, 0);
  windowLayout->addWidget(setFileButton_, 0, 0);
  windowLayout->addWidget(timeLabel_, 1, 0);
  windowLayout->addWidget(timeLineEdit_, 1, 1);
  windowLayout->addWidget(fileLineEdit_, 0, 1);

  window_->setLayout(windowLayout);

  // Connect event handling signals.
  QObject::connect(setFileButton_, &QPushButton::released, this,
                   &ConfigWindow::handleFileButton);

  QObject::connect(startButton_, &QPushButton::released, this,
                   &ConfigWindow::handleStartButton);
}

// ____________________________________________________________________________
void ConfigWindow::show() { window_->show(); }
void ConfigWindow::hide() { window_->hide(); }

// ____________________________________________________________________________
void ConfigWindow::handleFileButton() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Select data file"), "", tr(""));

  if (!fileName.isEmpty()) {
    fileLineEdit_->setText(fileName);
    qInfo() << "Selected data file: " << fileName;
  }
}

// ____________________________________________________________________________
void ConfigWindow::handleStartButton() {
  emit startButtonPressed(fileLineEdit_->text(), timeLineEdit_->text());
}

// ____________________________________________________________________________
void ConfigWindow::onStartLiveView(std::string fileName, float timeframe) {
  startButton_->setText("Stop");

  setFileButton_->setEnabled(false);
  fileLineEdit_->setEnabled(false);
  timeLineEdit_->setEnabled(false);
  timeLabel_->setEnabled(false);
}

// ____________________________________________________________________________
void ConfigWindow::onStopLiveView() {
  startButton_->setText("Start");

  setFileButton_->setEnabled(true);
  fileLineEdit_->setEnabled(true);
  timeLineEdit_->setEnabled(true);
  timeLabel_->setEnabled(true);
}

// ____________________________________________________________________________
OutputWindow::OutputWindow() {
  window_ = new QWidget();
  window_->setFixedSize(400, 200);

  QGridLayout *windowLayout = new QGridLayout;

  label_ = new QLabel("Plots ...");

  windowLayout->addWidget(label_, 0, 0);

  window_->setLayout(windowLayout);
}

// ____________________________________________________________________________
void OutputWindow::show() { window_->show(); }

// ____________________________________________________________________________
void OutputWindow::hide() { window_->hide(); }

// ____________________________________________________________________________
void OutputWindow::onStartLiveView(std::string fileName, float timeframe) {
  show();
}

// ____________________________________________________________________________
void OutputWindow::onStopLiveView() { hide(); }

// ____________________________________________________________________________
ForcePlateFeedback::ForcePlateFeedback() {
  running_ = false;
  configWindow_ = new ConfigWindow();
  outputWindow_ = new OutputWindow();
  dataModel_ = new DataModel();

  // Signal for start button pressed.
  QObject::connect(configWindow_, &ConfigWindow::startButtonPressed, this,
                   &ForcePlateFeedback::onStartButtonPressed);

  // State notification signals.
  // Start live view.
  QObject::connect(this, &ForcePlateFeedback::startLiveView, configWindow_,
                   &ConfigWindow::onStartLiveView);

  QObject::connect(this, &ForcePlateFeedback::startLiveView, outputWindow_,
                   &OutputWindow::onStartLiveView);

  QObject::connect(this, &ForcePlateFeedback::startLiveView, dataModel_,
                   &DataModel::onStartLiveView);

  // Stop live view.
  QObject::connect(this, &ForcePlateFeedback::stopLiveView, configWindow_,
                   &ConfigWindow::onStopLiveView);

  QObject::connect(this, &ForcePlateFeedback::stopLiveView, outputWindow_,
                   &OutputWindow::onStopLiveView);

  QObject::connect(this, &ForcePlateFeedback::stopLiveView, dataModel_,
                   &DataModel::onStopLiveView);
}

// ____________________________________________________________________________
ForcePlateFeedback::~ForcePlateFeedback() {
  delete configWindow_;
  delete outputWindow_;
  delete dataModel_;
}

// ____________________________________________________________________________
void ForcePlateFeedback::showConfigWindow() { configWindow_->show(); }

// ____________________________________________________________________________
void ForcePlateFeedback::onStartButtonPressed(QString fileName,
                                              QString timeframe) {
  if (running_) {
    running_ = false;
    // Notify ConfigWindow, OutputWindow and DataModel about the stop.
    qInfo() << "Stopping live view.";
    emit stopLiveView();
    return;
  }

  float timeframeFloat = timeframe.toFloat();

  if (!validateConfigOptions(fileName.toStdString(), timeframeFloat)) {
    // error message
  }
  fileName_ = fileName.toStdString();
  timeframe_ = timeframeFloat;

  running_ = true;

  // Initialize the data model.

  // Notify ConfigWindow, OutputWindow and DataModel about the start.
  emit startLiveView(fileName_, timeframe_);

  qInfo() << "Starting live view.";
}

// ____________________________________________________________________________
bool ForcePlateFeedback::validateConfigOptions(std::string fileName,
                                               float timeframe) {
  // Check if filename is not empty.

  // Check if time frame is not zero.

  return true;
}