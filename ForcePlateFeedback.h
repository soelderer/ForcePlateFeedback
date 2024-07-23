// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./KistlerFile.h"
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

#pragma once

#include <QtGui/QIntValidator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>

// Maximum timeframe in miliseconds over which the parameters may be calculated.
#define MAX_TIMEFRAME 10'000

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
class DataModel {

private:
  KistlerCSVFile kistlerFile_;
  BalanceParameters balanceParameters_;
};

// A class for the implementation of the output window.
// The window will show the plots to the participant.
class OutputWindow : public QWidget {
  Q_OBJECT

public:
  OutputWindow();
  ~OutputWindow() {}

  void preparePlot(BalanceParameters *balanceParameters) {}
  void refresh() {}
  void show();
  void hide();

private:
  // Qt stuff.
  QWidget *window_;
  QLabel *label_;

public slots:
  // Communication with ForcePlateFeedback class.
  void onStartLiveView();
  void onStopLiveView();
};

// A class for the implementation of the configuration window.
// This is the first window that the researcher sees, if she opens the app.
// There, she can make settings like setting the file to read from, and a
// timeframe. There will be buttons to control the flow of the program, like
// "Start", "Pause" etc.
// I'm not yet familiar enough with Qt to see if this makes sense, or if it's
// better to just directly use the respective Qt classes.
class ConfigWindow : public QWidget {
  Q_OBJECT

public:
  ConfigWindow();

  ~ConfigWindow() {}

  void refresh();
  void show();
  void hide();

private:
  // Qt stuff.
  QWidget *window_;
  QPushButton *startButton_;
  QPushButton *setFileButton_;
  QLabel *timeLabel_;
  QLineEdit *timeLineEdit_;
  QLineEdit *fileLineEdit_;
  QFileDialog *fileDialog_;

private slots:
  // Event handlers.
  void handleStartButton();
  void handleFileButton();

public slots:
  // Communication with ForcePlateFeedback class.
  void onStartLiveView();
  void onStopLiveView();

signals:
  void startButtonPressed(QString fileName, QString timeframe);
};

// A class for the coordination of GUI and core logic. Sets up the
// Qt application, connects signals etc.
class ForcePlateFeedback : public QWidget {
  Q_OBJECT

public:
  ForcePlateFeedback();

  ~ForcePlateFeedback();

  // Shows the config window.
  void showConfigWindow();

private:
  // The GUI layer.
  ConfigWindow *configWindow_;
  OutputWindow *outputWindow_;

  // The data model responsible for continuously calculating the parameters.
  DataModel *dataModel_;

  // Configuration options.
  std::string fileName_;
  float timeframe_;

  // State of the program (running / paused).
  bool running_;

  // Sanity checks / validation of config options. Returns true if the config
  // options are valid.
  bool validateConfigOptions(std::string fileName, float timeframe);

signals:
  void startLiveView();
  void stopLiveView();

private slots:
  // Start button was pressed.
  void onStartButtonPressed(QString fileName, QString timeframe);
};