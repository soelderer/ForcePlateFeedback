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
  fileDialog_ = new QFileDialog();

  windowLayout->addWidget(startButton_, 2, 0);
  windowLayout->addWidget(setFileButton_, 0, 0);
  windowLayout->addWidget(timeLabel_, 1, 0);
  windowLayout->addWidget(timeLineEdit_, 1, 1);
  windowLayout->addWidget(fileLineEdit_, 0, 1);

  window_->setLayout(windowLayout);

  // Connect event handling signals
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
    fileName_ = fileName.toStdString();
    std::cout << "Selected data file: " << fileName.toStdString() << std::endl;
  }
}

// ____________________________________________________________________________
void ConfigWindow::handleStartButton() { emit startPressed(); }

// ____________________________________________________________________________
ForcePlateFeedback::ForcePlateFeedback() {
  running_ = false;
  configWindow_ = new ConfigWindow();

  // Connect signals.
  QObject::connect(configWindow_, &ConfigWindow::startPressed, this,
                   &ForcePlateFeedback::onStartPressed);
}

// ____________________________________________________________________________
ForcePlateFeedback::~ForcePlateFeedback() { delete configWindow_; }

// ____________________________________________________________________________
void ForcePlateFeedback::showConfigWindow() { configWindow_->show(); }

// ____________________________________________________________________________
void ForcePlateFeedback::onStartPressed() {
  std::cout << "Starting live view." << std::endl;

  // sanity checks ...

  running_ = true;
}