// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#pragma once

#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

#include "./DataModel.h"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QValueAxis>
#include <QtGui/QIntValidator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>

// Maximum timeframe in miliseconds over which the parameters may be calculated.
#define MAX_TIMEFRAME 10'000

// A class for the implementation of the output window.
// The window will show the plots to the participant.
class OutputWindow : public QWidget {
  Q_OBJECT

public:
  OutputWindow();
  // No custom destructors needed, as the Qt object tree takes care of
  // widget destruction, https://stackoverflow.com/a/37056051

  // Qt objects are not supposed to be copied, so no copy constructor and
  // assignment operator implemented. See https://stackoverflow.com/a/19092698

  void show();
  void hide();

private:
  // Qt stuff.
  QWidget *window_;
  QLabel *label_;

  // The plot.
  QChart *xChart_;
  QBarSeries *xSeries_;
  QBarSet *xSet_;
  QStringList xCategories_;
  QBarCategoryAxis *xAxisX_;
  QValueAxis *xAxisY_;
  QChartView *xChartView_;
  QChart *yChart_;
  QHorizontalBarSeries *ySeries_;
  QBarSet *ySet_;
  QStringList yCategories_;
  QBarCategoryAxis *yAxisY_;
  QValueAxis *yAxisX_;
  QChartView *yChartView_;

public slots:
  // Communication with ForcePlateFeedback class. These functions are connected
  // to signals. Qt takes care of calling these slots when the signals are
  // elicited.
  void onStartLiveView(const std::string &fileName, const float timeframe);
  void onStopLiveView();
  void onDataUpdated(const BalanceParameters *balanceParameters);

  // On const-correctness of signals:
  // https://stackoverflow.com/questions/39281740/why-are-qt-signals-not-const
};

// A class for the implementation of the configuration window.
// This is the first window that the researcher sees, if she opens the app.
// There, she can make settings like setting the file to read from, and a
// timeframe. There will be buttons to control the flow of the program, like
// "Start", "Pause" etc.
class ConfigWindow : public QWidget {
  Q_OBJECT

public:
  ConfigWindow();

  // Qt objects are not supposed to be copied, so no copy constructor and
  // assignment operator implemented. See https://stackoverflow.com/a/19092698

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
  // Event handlers for start button and file selection dialog.
  void handleStartButton();
  void handleFileButton();

public slots:
  // Communication with ForcePlateFeedback class. These functions are connected
  // to signals. Qt takes care of calling these slots when the signals are
  // elicited.
  void onStartLiveView(const std::string &fileName, const float timeframe);
  void onStopLiveView();

signals:
  // Communication with ForcePlateFeedback class. This signal is emitted when
  // the start button is pressed. The actual logic is in ForcePlateFeedback.
  void startButtonPressed(const QString &fileName, const QString &timeframe);
};

// Class which abstracts QMessageBox such that a mock message dialog can be
// used in unit tests. Default handler is a simple QMessageBox, the mock handler
// just prints to stdout.
class MessageHandler {
public:
  virtual ~MessageHandler() {}
  virtual void showDialog(const QString &message) = 0;
};

class DefaultMessageHandler : public MessageHandler {
public:
  void showDialog(const QString &message) override {
    QMessageBox messageDialog;
    messageDialog.setText(message);
    messageDialog.exec();
  }
};

class MockMessageHandler : public MessageHandler {
public:
  void showDialog(const QString &message) override {
    std::cout << "MessageBox with: " << message.toStdString() << std::endl;
  }
};

// A class for the coordination of GUI and core logic. Sets up the
// Qt application, connects signals and slots etc.
class ForcePlateFeedback : public QWidget {
  Q_OBJECT

public:
  ForcePlateFeedback();
  ~ForcePlateFeedback();
  // Qt objects are not supposed to be copied, so no copy constructor and
  // assignment operator implemented. See https://stackoverflow.com/a/19092698

  // Shows the config window.
  void showConfigWindow();

private:
  // The GUI layer.
  ConfigWindow *configWindow_;
  OutputWindow *outputWindow_;

  // The data model responsible for continuously calculating the parameters.
  DataModel *dataModel_;

  // Class for handling message dialogs. This allows a mock handler for unit
  // tests via dependency injection.
  MessageHandler *messageHandler_;

  // Configuration options.
  std::string fileName_;
  float timeframe_;

  // State of the program (running / paused).
  bool running_;

  // Sanity checks / validation of config options. Returns true if the config
  // options are valid.
  static bool validateConfigOptions(const std::string &fileName,
                                    const float timeframe);

  FRIEND_TEST(ForcePlateFeedbackTest, validateConfigOptions);
  FRIEND_TEST(ForcePlateFeedbackTest, combinedTest);

  // Start and stop the live view. Takes care of emitting the right signals,
  // changing state variables etc.
  void startLiveView(const QString &fileName, const QString &timeframe);
  void stopLiveView();

signals:
  // These signals are emitted to start and stop the live view. The
  // corresponding slots in the OutputWindow, DataModel etc. take care of the
  // neccessary changes to GUI and data model.
  void startLiveViewSignal(const std::string &fileName, const float timeframe);
  void stopLiveViewSignal();
  void resetModel();

private slots:
  // Start button was pressed.
  void onStartButtonPressed(const QString &fileName, const QString &timeframe);

public slots:
  // These slots are called by the DataModel on end of file, invalid file, etc.
  void onReachedEOF();
  void onInvalidFile();
  void onCorruptFile();
};