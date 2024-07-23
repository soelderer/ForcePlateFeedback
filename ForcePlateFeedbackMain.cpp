// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./ForcePlateFeedback.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  ForcePlateFeedback forcePlateFeedback;
  forcePlateFeedback.showConfigWindow();

  return app.exec();
}