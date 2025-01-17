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
void ConfigWindow::onStartLiveView(const std::string &fileName,
                                   const float timeframe) {
  // can't get rid of "unused parameters" because signatures must match the
  // signal

  startButton_->setText("Pause");

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
  window_->setFixedSize(1200, 600);

  QGridLayout *windowLayout = new QGridLayout;

  // Chart for Force in X direction.
  xChart_ = new QChart();
  xSeries_ = new QBarSeries();
  xSet_ = new QBarSet("Values");

  *xSet_ << 0;

  xSeries_->append(xSet_);

  xChart_->addSeries(xSeries_);
  xChart_->setAnimationOptions(QChart::NoAnimation);
  xChart_->setTheme(QChart::ChartThemeBlueIcy);

  xCategories_.append("X");

  xAxisX_ = new QBarCategoryAxis();
  xAxisX_->append(xCategories_);
  xChart_->addAxis(xAxisX_, Qt::AlignBottom);
  xSeries_->attachAxis(xAxisX_);

  xAxisY_ = new QValueAxis();
  xAxisY_->setRange(-80, 80); // in the test file the data is out of range
                              // sometimes, we will have to tweak this to see
                              // what makes sense for our practical use
  xChart_->addAxis(xAxisY_, Qt::AlignLeft);
  xSeries_->attachAxis(xAxisY_);

  // No legend
  xChart_->legend()->setVisible(false);

  // Create a chart view and set the chart
  xChartView_ = new QChartView(xChart_);
  xChartView_->setRenderHint(QPainter::Antialiasing);

  // Chart for force in Y direction.
  yChart_ = new QChart();
  ySeries_ = new QHorizontalBarSeries();
  ySet_ = new QBarSet("Values");

  *ySet_ << 0;

  ySeries_->append(ySet_);

  yChart_->addSeries(ySeries_);
  yChart_->setAnimationOptions(QChart::NoAnimation);
  yChart_->setTheme(QChart::ChartThemeBlueIcy);

  yCategories_.append("Y");

  yAxisX_ = new QValueAxis();
  yAxisX_->setRange(-80, 80); // in the test file the data is out of range
                              // sometimes, we will have to tweak this to see
                              // what makes sense for our practical use
  yChart_->addAxis(yAxisX_, Qt::AlignBottom);
  ySeries_->attachAxis(yAxisX_);

  yAxisY_ = new QBarCategoryAxis();
  yAxisY_->append(yCategories_);
  yChart_->addAxis(yAxisY_, Qt::AlignLeft);
  ySeries_->attachAxis(yAxisY_);

  // No legend
  yChart_->legend()->setVisible(false);

  // Create a chart view and set the chart
  yChartView_ = new QChartView(yChart_);
  yChartView_->setRenderHint(QPainter::Antialiasing);

  windowLayout->addWidget(xChartView_, 0, 0);
  windowLayout->addWidget(yChartView_, 0, 1);

  window_->setLayout(windowLayout);
}

// ____________________________________________________________________________
void OutputWindow::show() { window_->show(); }

// ____________________________________________________________________________
void OutputWindow::hide() { window_->hide(); }

// ____________________________________________________________________________
void OutputWindow::onStartLiveView(const std::string &fileName,
                                   const float timeframe) {
  // can't get rid of "unused parameters" because signatures must match the
  // signal

  show();
}

// ____________________________________________________________________________
void OutputWindow::onStopLiveView() { hide(); }

// ____________________________________________________________________________
void OutputWindow::onDataUpdated(const BalanceParameters *balanceParameters) {
  xSet_->replace(0, balanceParameters->getMeanForceX());
  ySet_->replace(0, balanceParameters->getMeanForceY());

  xChartView_->repaint();
  yChartView_->repaint();
}

// ____________________________________________________________________________
ForcePlateFeedback::ForcePlateFeedback() {
  running_ = false;
  fileName_ = "";
  timeframe_ = 0;
  configWindow_ = new ConfigWindow();
  outputWindow_ = new OutputWindow();
  dataModel_ = new DataModel();
  messageHandler_ = new DefaultMessageHandler();

  // Signal for start button pressed.
  QObject::connect(configWindow_, &ConfigWindow::startButtonPressed, this,
                   &ForcePlateFeedback::onStartButtonPressed);

  // State notification signals.
  // Start live view.
  QObject::connect(this, &ForcePlateFeedback::startLiveViewSignal,
                   configWindow_, &ConfigWindow::onStartLiveView);

  QObject::connect(this, &ForcePlateFeedback::startLiveViewSignal,
                   outputWindow_, &OutputWindow::onStartLiveView);

  QObject::connect(this, &ForcePlateFeedback::startLiveViewSignal, dataModel_,
                   &DataModel::onStartProcessing);

  // Stop live view.
  QObject::connect(this, &ForcePlateFeedback::stopLiveViewSignal, configWindow_,
                   &ConfigWindow::onStopLiveView);

  QObject::connect(this, &ForcePlateFeedback::stopLiveViewSignal, outputWindow_,
                   &OutputWindow::onStopLiveView);

  QObject::connect(this, &ForcePlateFeedback::stopLiveViewSignal, dataModel_,
                   &DataModel::onStopProcessing);

  // Data updated.
  QObject::connect(dataModel_, &DataModel::dataUpdated, outputWindow_,
                   &OutputWindow::onDataUpdated);

  // Reached EOF.
  QObject::connect(dataModel_, &DataModel::reachedEOF, this,
                   &ForcePlateFeedback::onReachedEOF);

  // Reset DataModel.
  QObject::connect(this, &ForcePlateFeedback::resetModel, dataModel_,
                   &DataModel::onResetModel);

  // Invalid file selected.
  QObject::connect(dataModel_, &DataModel::invalidFileSignal, this,
                   &ForcePlateFeedback::onInvalidFile);

  // Corrupt file (low-level errors while processing the file).
  QObject::connect(dataModel_, &DataModel::corruptFileSignal, this,
                   &ForcePlateFeedback::onCorruptFile);
}

// ____________________________________________________________________________
ForcePlateFeedback::~ForcePlateFeedback() {
  delete outputWindow_;
  delete configWindow_;
  delete dataModel_;
  delete messageHandler_;
}

// ____________________________________________________________________________
void ForcePlateFeedback::showConfigWindow() { configWindow_->show(); }

// ____________________________________________________________________________
void ForcePlateFeedback::startLiveView(const QString &fileName,
                                       const QString &timeframe) {
  if (!running_) {
    float timeframeFloat = timeframe.toFloat();

    if (!validateConfigOptions(fileName.toStdString(), timeframeFloat)) {
      messageHandler_->showDialog(
          "Please select a file and enter a non-zero timeframe.");
      return;
    }

    fileName_ = fileName.toStdString();
    timeframe_ = timeframeFloat / 1000; // ms to s

    running_ = true;

    // Notify ConfigWindow, OutputWindow and DataModel about the start.
    qInfo() << "Starting live view.";
    emit startLiveViewSignal(fileName_, timeframe_);
  }
}

// ____________________________________________________________________________
void ForcePlateFeedback::stopLiveView() {
  if (running_) {
    running_ = false;
    // Notify ConfigWindow, OutputWindow and DataModel about the stop.
    qInfo() << "Stopping live view.";
    emit stopLiveViewSignal();
  }
}

// ____________________________________________________________________________
void ForcePlateFeedback::onStartButtonPressed(const QString &fileName,
                                              const QString &timeframe) {
  if (running_)
    stopLiveView();
  else
    startLiveView(fileName, timeframe);
}

// ____________________________________________________________________________
bool ForcePlateFeedback::validateConfigOptions(const std::string &fileName,
                                               const float timeframe) {
  if (fileName.empty())
    return false;

  if (timeframe <= 0)
    return false;

  return true;
}

// ____________________________________________________________________________
void ForcePlateFeedback::onReachedEOF() {
  stopLiveView();

  // Reset the DataModel.
  qInfo() << "Resetting the DataModel.";
  emit resetModel();
}

// ____________________________________________________________________________
void ForcePlateFeedback::onInvalidFile() {
  stopLiveView();

  messageHandler_->showDialog(
      "File does not "
      "appear to be a valid BioWare file. Please double-check.");
}

// ____________________________________________________________________________
void ForcePlateFeedback::onCorruptFile() {
  stopLiveView();

  messageHandler_->showDialog(
      "Stumbled upon invalid data while processing the "
      "file. Seems like the data is corrupt. Aborting.");
}