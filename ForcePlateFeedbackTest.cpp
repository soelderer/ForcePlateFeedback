// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./ForcePlateFeedback.h"
#include <gtest/gtest.h>
// ____________________________________________________________________________
// I couldn't test the elicitation of the signals with gtest. Therefore, unit
// tests for the signals are missing. However, the logic is in the slots, which
// I was able to unit test. With the QTest framework, you could test elicitation
// of the signals via a signal counter.
// ____________________________________________________________________________

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, validateFile) {
  // A proper file.
  KistlerCSVFile kistlerFile("./example_data/KistlerCSV_example.txt");
  ASSERT_EQ(kistlerFile.getFilename(), "./example_data/KistlerCSV_example.txt");

  kistlerFile.validateFile();
  ASSERT_TRUE(kistlerFile.isValid());

  // An empty file.
  kistlerFile = KistlerCSVFile("./example_data/KistlerCSV_empty.txt");
  ASSERT_EQ(kistlerFile.getFilename(), "./example_data/KistlerCSV_empty.txt");

  kistlerFile.validateFile();
  ASSERT_FALSE(kistlerFile.isValid());

  // A file with missing magic number.
  kistlerFile = KistlerCSVFile("./example_data/KistlerCSV_wrong_magic.txt");
  ASSERT_EQ(kistlerFile.getFilename(),
            "./example_data/KistlerCSV_wrong_magic.txt");

  kistlerFile.validateFile();
  ASSERT_FALSE(kistlerFile.isValid());

  // A file with improper column headers.
  kistlerFile = KistlerCSVFile("./example_data/KistlerCSV_wrong_column.txt");
  ASSERT_EQ(kistlerFile.getFilename(),
            "./example_data/KistlerCSV_wrong_column.txt");

  kistlerFile.validateFile();
  ASSERT_FALSE(kistlerFile.isValid());
}

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, sliceRow) {
  // Regular case
  std::string str("one\ttwo\tthree\tfour");
  auto strings = KistlerCSVFile::sliceRow(str, '\t');

  ASSERT_EQ(strings.size(), 4);
  ASSERT_EQ(strings[0], "one");
  ASSERT_EQ(strings[1], "two");
  ASSERT_EQ(strings[2], "three");
  ASSERT_EQ(strings[3], "four");

  // Two consecutive delimiters should result in an empty cell.
  str = "one\t\ttwo\tthree\tfour";
  strings = KistlerCSVFile::sliceRow(str, '\t');

  ASSERT_EQ(strings.size(), 5);
  ASSERT_EQ(strings[0], "one");
  ASSERT_EQ(strings[1], "");
  ASSERT_EQ(strings[2], "two");
  ASSERT_EQ(strings[3], "three");
  ASSERT_EQ(strings[4], "four");

  // Three consecutive delimiters should result in two empty cells.
  str = "one\t\t\ttwo\tthree\tfour";
  strings = KistlerCSVFile::sliceRow(str, '\t');

  ASSERT_EQ(strings.size(), 6);
  ASSERT_EQ(strings[0], "one");
  ASSERT_EQ(strings[1], "");
  ASSERT_EQ(strings[2], "");
  ASSERT_EQ(strings[3], "two");
  ASSERT_EQ(strings[4], "three");
  ASSERT_EQ(strings[5], "four");

  // Delimiters in the beginning and end of the line should result in empty
  // cells.
  str = "\tone\ttwo\tthree\tfour\t";
  strings = KistlerCSVFile::sliceRow(str, '\t');

  ASSERT_EQ(strings.size(), 6);
  ASSERT_EQ(strings[0], "");
  ASSERT_EQ(strings[1], "one");
  ASSERT_EQ(strings[2], "two");
  ASSERT_EQ(strings[3], "three");
  ASSERT_EQ(strings[4], "four");
  ASSERT_EQ(strings[5], "");

  // Multiple delimiters in the beginning and end of the line should result in
  // multiple empty cells.
  str = "\t\tone\ttwo\tthree\tfour\t\t";
  strings = KistlerCSVFile::sliceRow(str, '\t');

  ASSERT_EQ(strings.size(), 8);
  ASSERT_EQ(strings[0], "");
  ASSERT_EQ(strings[1], "");
  ASSERT_EQ(strings[2], "one");
  ASSERT_EQ(strings[3], "two");
  ASSERT_EQ(strings[4], "three");
  ASSERT_EQ(strings[5], "four");
  ASSERT_EQ(strings[6], "");
  ASSERT_EQ(strings[7], "");

  // Test a different delimiter.
  str = ";;one;two;three;four;;";
  strings = KistlerCSVFile::sliceRow(str, ';');

  ASSERT_EQ(strings.size(), 8);
  ASSERT_EQ(strings[0], "");
  ASSERT_EQ(strings[1], "");
  ASSERT_EQ(strings[2], "one");
  ASSERT_EQ(strings[3], "two");
  ASSERT_EQ(strings[4], "three");
  ASSERT_EQ(strings[5], "four");
  ASSERT_EQ(strings[6], "");
  ASSERT_EQ(strings[7], "");
}

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, parseMetaData) {
  // Regular case.
  KistlerCSVFile kistlerFile("example_data/KistlerCSV_example.txt");
  // Constructor calls parseMetaData(), so let's reset the column names
  // and explicitly call parseMetaData.
  kistlerFile.columnNames_ = std::vector<std::string>();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 0);
  kistlerFile.parseMetaData();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 9);
  ASSERT_STREQ(kistlerFile.columnNames_[0].c_str(), "abs time (s)");
  ASSERT_STREQ(kistlerFile.columnNames_[1].c_str(), "Fx");
  ASSERT_STREQ(kistlerFile.columnNames_[2].c_str(), "Fy");
  ASSERT_STREQ(kistlerFile.columnNames_[3].c_str(), "Fz");
  ASSERT_STREQ(kistlerFile.columnNames_[4].c_str(), "Mx");
  ASSERT_STREQ(kistlerFile.columnNames_[5].c_str(), "My");
  ASSERT_STREQ(kistlerFile.columnNames_[6].c_str(), "Mz");
  ASSERT_STREQ(kistlerFile.columnNames_[7].c_str(), "Ax");
  ASSERT_STREQ(kistlerFile.columnNames_[8].c_str(), "Ay");
  ASSERT_FLOAT_EQ(kistlerFile.getSamplingRate(), 1000.0);
  ASSERT_TRUE(kistlerFile.isValid());

  // Other column names.
  kistlerFile = KistlerCSVFile("example_data/KistlerCSV_wrong_column.txt");
  kistlerFile.columnNames_ = std::vector<std::string>();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 0);
  // parseMetaData checks for isValid_, so we need to override the variable for
  // the test to work.
  kistlerFile.isValid_ = true;
  kistlerFile.parseMetaData();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 9);
  ASSERT_STREQ(kistlerFile.columnNames_[0].c_str(), "abs iitime  wef (s)");
  ASSERT_STREQ(kistlerFile.columnNames_[1].c_str(), "Fx");
  ASSERT_STREQ(kistlerFile.columnNames_[2].c_str(), "Fy");
  ASSERT_STREQ(kistlerFile.columnNames_[3].c_str(), "Fz");
  ASSERT_STREQ(kistlerFile.columnNames_[4].c_str(), "Mx");
  ASSERT_STREQ(kistlerFile.columnNames_[5].c_str(), "My");
  ASSERT_STREQ(kistlerFile.columnNames_[6].c_str(), " jklöklklölökMz");
  ASSERT_STREQ(kistlerFile.columnNames_[7].c_str(), "Ax");
  ASSERT_STREQ(kistlerFile.columnNames_[8].c_str(), "Ay 203 jlkd");
  ASSERT_FLOAT_EQ(kistlerFile.getSamplingRate(), 1000.0);
  ASSERT_TRUE(kistlerFile.isValid());

  // Invalid sampling rate.
  kistlerFile =
      KistlerCSVFile("example_data/KistlerCSV_wrong_samplingrate.txt");
  kistlerFile.columnNames_ = std::vector<std::string>();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 0);
  kistlerFile.parseMetaData();
  ASSERT_EQ(
      kistlerFile.columnNames_.size(),
      0); // columns are parsed after sampling rate, function quits when invalid
  ASSERT_FALSE(kistlerFile.isValid());
  ASSERT_FLOAT_EQ(kistlerFile.getSamplingRate(), 0.0);
}

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, kistlerCSVFileDefaultConstructor) {
  KistlerCSVFile kistlerFile;
  ASSERT_FALSE(kistlerFile.isValid());
  ASSERT_STREQ(kistlerFile.getFilename().c_str(), "");
  ASSERT_FLOAT_EQ(kistlerFile.getSamplingRate(), 0);
}

// I wasn't sure how to test the non-default constructor without code
// duplication from validateFile() and parseMetaData() ... essentially, these
// two functions are a test of the constructor, because I check for the right
// filename, sampling rate, column names.

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, getDataByIndices) {
  KistlerCSVFile kistlerFile("example_data/KistlerCSV_example.txt");
  // Get the first row.
  auto data = kistlerFile.getData(0, 0);
  ASSERT_EQ(data->size(), 9);
  ASSERT_EQ(data->at("abs time (s)").size(), 1);
  ASSERT_EQ(data->at("Fx").size(), 1);
  ASSERT_EQ(data->at("Fy").size(), 1);
  ASSERT_EQ(data->at("Fz").size(), 1);
  ASSERT_EQ(data->at("Mx").size(), 1);
  ASSERT_EQ(data->at("My").size(), 1);
  ASSERT_EQ(data->at("Mz").size(), 1);
  ASSERT_EQ(data->at("Ax").size(), 1);
  ASSERT_EQ(data->at("Ay").size(), 1);
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[0], 0);
  ASSERT_FLOAT_EQ(data->at("Fx")[0], 0.145133);
  ASSERT_FLOAT_EQ(data->at("Fy")[0], -0.010285);
  ASSERT_FLOAT_EQ(data->at("Fz")[0], -0.126362);
  ASSERT_FLOAT_EQ(data->at("Mx")[0], -0.362161);
  ASSERT_FLOAT_EQ(data->at("My")[0], 0.150046);
  ASSERT_FLOAT_EQ(data->at("Mz")[0], 0.001693);
  ASSERT_FLOAT_EQ(data->at("Ax")[0], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[0], 0);

  // Get rows 9 to 12.
  data = kistlerFile.getData(8, 11);
  ASSERT_EQ(data->size(), 9);
  ASSERT_EQ(data->at("abs time (s)").size(), 4);
  ASSERT_EQ(data->at("Fx").size(), 4);
  ASSERT_EQ(data->at("Fy").size(), 4);
  ASSERT_EQ(data->at("Fz").size(), 4);
  ASSERT_EQ(data->at("Mx").size(), 4);
  ASSERT_EQ(data->at("My").size(), 4);
  ASSERT_EQ(data->at("Mz").size(), 4);
  ASSERT_EQ(data->at("Ax").size(), 4);
  ASSERT_EQ(data->at("Ay").size(), 4);
  // Row 9
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[0], 0.008);
  ASSERT_FLOAT_EQ(data->at("Fx")[0], -0.011207);
  ASSERT_FLOAT_EQ(data->at("Fy")[0], -0.205451);
  ASSERT_FLOAT_EQ(data->at("Fz")[0], -1.102404);
  ASSERT_FLOAT_EQ(data->at("Mx")[0], 0.238696);
  ASSERT_FLOAT_EQ(data->at("My")[0], 0.348605);
  ASSERT_FLOAT_EQ(data->at("Mz")[0], -0.067442);
  ASSERT_FLOAT_EQ(data->at("Ax")[0], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[0], 0);
  // Row 10
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[1], 0.009);
  ASSERT_FLOAT_EQ(data->at("Fx")[1], 0.145173);
  ASSERT_FLOAT_EQ(data->at("Fy")[1], -0.049630);
  ASSERT_FLOAT_EQ(data->at("Fz")[1], -1.428445);
  ASSERT_FLOAT_EQ(data->at("Mx")[1], 0.147898);
  ASSERT_FLOAT_EQ(data->at("My")[1], -0.124266);
  ASSERT_FLOAT_EQ(data->at("Mz")[1], 0.022363);
  ASSERT_FLOAT_EQ(data->at("Ax")[1], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[1], 0);
  // Row 11
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[2], 0.01);
  ASSERT_FLOAT_EQ(data->at("Fx")[2], -0.050342);
  ASSERT_FLOAT_EQ(data->at("Fy")[2], -0.088255);
  ASSERT_FLOAT_EQ(data->at("Fz")[2], -0.288165);
  ASSERT_FLOAT_EQ(data->at("Mx")[2], 0.192003);
  ASSERT_FLOAT_EQ(data->at("My")[2], 0.039073);
  ASSERT_FLOAT_EQ(data->at("Mz")[2], -0.065504);
  ASSERT_FLOAT_EQ(data->at("Ax")[2], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[2], 0);
  // Row 12
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[3], 0.011);
  ASSERT_FLOAT_EQ(data->at("Fx")[3], 0.066863);
  ASSERT_FLOAT_EQ(data->at("Fy")[3], -0.010165);
  ASSERT_FLOAT_EQ(data->at("Fz")[3], 0.364267);
  ASSERT_FLOAT_EQ(data->at("Mx")[3], 0.019169);
  ASSERT_FLOAT_EQ(data->at("My")[3], 0.249212);
  ASSERT_FLOAT_EQ(data->at("Mz")[3], 0.005645);
  ASSERT_FLOAT_EQ(data->at("Ax")[3], -0.684146);
  ASSERT_FLOAT_EQ(data->at("Ay")[3], 0.052623);

  // Get rows from the beginning to line 2.
  data = kistlerFile.getData(-1, 1);
  ASSERT_EQ(data->size(), 9);
  ASSERT_EQ(data->at("abs time (s)").size(), 2);
  ASSERT_EQ(data->at("Fx").size(), 2);
  ASSERT_EQ(data->at("Fy").size(), 2);
  ASSERT_EQ(data->at("Fz").size(), 2);
  ASSERT_EQ(data->at("Mx").size(), 2);
  ASSERT_EQ(data->at("My").size(), 2);
  ASSERT_EQ(data->at("Mz").size(), 2);
  ASSERT_EQ(data->at("Ax").size(), 2);
  ASSERT_EQ(data->at("Ay").size(), 2);
  // Row 1.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[0], 0);
  ASSERT_FLOAT_EQ(data->at("Fx")[0], 0.145133);
  ASSERT_FLOAT_EQ(data->at("Fy")[0], -0.010285);
  ASSERT_FLOAT_EQ(data->at("Fz")[0], -0.126362);
  ASSERT_FLOAT_EQ(data->at("Mx")[0], -0.362161);
  ASSERT_FLOAT_EQ(data->at("My")[0], 0.150046);
  ASSERT_FLOAT_EQ(data->at("Mz")[0], 0.001693);
  ASSERT_FLOAT_EQ(data->at("Ax")[0], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[0], 0);
  // Row 2.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[1], 0.001);
  ASSERT_FLOAT_EQ(data->at("Fx")[1], -0.011368);
  ASSERT_FLOAT_EQ(data->at("Fy")[1], -0.127600);
  ASSERT_FLOAT_EQ(data->at("Fz")[1], -1.756052);
  ASSERT_FLOAT_EQ(data->at("Mx")[1], -0.102902);
  ASSERT_FLOAT_EQ(data->at("My")[1], -0.404231);
  ASSERT_FLOAT_EQ(data->at("Mz")[1], 0.046690);
  ASSERT_FLOAT_EQ(data->at("Ax")[1], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[1], 0);

  // Get rows from line 30 to EOF.
  data = kistlerFile.getData(29, -1);
  ASSERT_EQ(data->size(), 9);
  ASSERT_EQ(data->at("abs time (s)").size(), 2);
  ASSERT_EQ(data->at("Fx").size(), 2);
  ASSERT_EQ(data->at("Fy").size(), 2);
  ASSERT_EQ(data->at("Fz").size(), 2);
  ASSERT_EQ(data->at("Mx").size(), 2);
  ASSERT_EQ(data->at("My").size(), 2);
  ASSERT_EQ(data->at("Mz").size(), 2);
  ASSERT_EQ(data->at("Ax").size(), 2);
  ASSERT_EQ(data->at("Ay").size(), 2);
  // Row 30.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[0], 0.029);
  ASSERT_FLOAT_EQ(data->at("Fx")[0], -0.050422);
  ASSERT_FLOAT_EQ(data->at("Fy")[0], 0.145775);
  ASSERT_FLOAT_EQ(data->at("Fz")[0], 0.852810);
  ASSERT_FLOAT_EQ(data->at("Mx")[0], -0.029384);
  ASSERT_FLOAT_EQ(data->at("My")[0], 0.141809);
  ASSERT_FLOAT_EQ(data->at("Mz")[0], 0.007898);
  ASSERT_FLOAT_EQ(data->at("Ax")[0], -0.166285);
  ASSERT_FLOAT_EQ(data->at("Ay")[0], -0.034456);
  // Row 31.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[1], 0.03);
  ASSERT_FLOAT_EQ(data->at("Fx")[1], -0.011408);
  ASSERT_FLOAT_EQ(data->at("Fy")[1], -0.205690);
  ASSERT_FLOAT_EQ(data->at("Fz")[1], -0.125492);
  ASSERT_FLOAT_EQ(data->at("Mx")[1], 0.069752);
  ASSERT_FLOAT_EQ(data->at("My")[1], 0.006508);
  ASSERT_FLOAT_EQ(data->at("Mz")[1], 0.067055);
  ASSERT_FLOAT_EQ(data->at("Ax")[1], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[1], 0);

  // Read a whole file.
  kistlerFile = KistlerCSVFile("example_data/KistlerCSV_stub.txt");
  data = kistlerFile.getData(-1, -1);
  ASSERT_EQ(data->size(), 9);
  ASSERT_EQ(data->at("abs time (s)").size(), 2);
  ASSERT_EQ(data->at("Fx").size(), 2);
  ASSERT_EQ(data->at("Fy").size(), 2);
  ASSERT_EQ(data->at("Fz").size(), 2);
  ASSERT_EQ(data->at("Mx").size(), 2);
  ASSERT_EQ(data->at("My").size(), 2);
  ASSERT_EQ(data->at("Mz").size(), 2);
  ASSERT_EQ(data->at("Ax").size(), 2);
  ASSERT_EQ(data->at("Ay").size(), 2);
  // Row 30.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[0], 0.029);
  ASSERT_FLOAT_EQ(data->at("Fx")[0], -0.050422);
  ASSERT_FLOAT_EQ(data->at("Fy")[0], 0.145775);
  ASSERT_FLOAT_EQ(data->at("Fz")[0], 0.852810);
  ASSERT_FLOAT_EQ(data->at("Mx")[0], -0.029384);
  ASSERT_FLOAT_EQ(data->at("My")[0], 0.141809);
  ASSERT_FLOAT_EQ(data->at("Mz")[0], 0.007898);
  ASSERT_FLOAT_EQ(data->at("Ax")[0], -0.166285);
  ASSERT_FLOAT_EQ(data->at("Ay")[0], -0.034456);
  // Row 31.
  ASSERT_FLOAT_EQ(data->at("abs time (s)")[1], 0.03);
  ASSERT_FLOAT_EQ(data->at("Fx")[1], -0.011408);
  ASSERT_FLOAT_EQ(data->at("Fy")[1], -0.205690);
  ASSERT_FLOAT_EQ(data->at("Fz")[1], -0.125492);
  ASSERT_FLOAT_EQ(data->at("Mx")[1], 0.069752);
  ASSERT_FLOAT_EQ(data->at("My")[1], 0.006508);
  ASSERT_FLOAT_EQ(data->at("Mz")[1], 0.067055);
  ASSERT_FLOAT_EQ(data->at("Ax")[1], 0);
  ASSERT_FLOAT_EQ(data->at("Ay")[1], 0);
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, defaultConstructor) {
  BalanceParameters balanceParameters;
  ASSERT_FALSE(balanceParameters.isValid());
  ASSERT_FLOAT_EQ(balanceParameters.getTimeframe(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getStartTime(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getStopTime(), 0);
  ASSERT_EQ(balanceParameters.getNumRows(), 0);
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, calculateMeanForceX) {
  auto data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();

  // Empty vector should yield an average of 0.
  (*data)["Fx"];
  BalanceParameters balanceParameters;
  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceX();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceX_, 0);

  // Some trivial example.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fx"].push_back(1);
  (*data)["Fx"].push_back(2);
  (*data)["Fx"].push_back(3);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceX();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceX_, 2);

  // Negatives.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fx"].push_back(-1);
  (*data)["Fx"].push_back(2);
  (*data)["Fx"].push_back(3);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceX();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceX_, 1.0 * 4 / 3);

  // More realistic data.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceX();
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceX(), 0.0317253);
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, calculateMeanForceY) {
  auto data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();

  // Empty vector should yield an average of 0.
  (*data)["Fy"];
  BalanceParameters balanceParameters;
  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceY();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceY_, 0);

  // Some trivial example.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fy"].push_back(1);
  (*data)["Fy"].push_back(2);
  (*data)["Fy"].push_back(3);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceY();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceY_, 2);

  // Negatives.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fy"].push_back(-1);
  (*data)["Fy"].push_back(2);
  (*data)["Fy"].push_back(3);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceY();
  ASSERT_FLOAT_EQ(balanceParameters.meanForceY_, 1.0 * 4 / 3);

  // More realistic data.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(-0.011207);
  (*data)["Fy"].push_back(0.145173);

  balanceParameters.data_ = data;
  balanceParameters.calculateMeanForceY();
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceY(), 0.0317253);
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, validateData) {
  // Regular case.
  auto data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(-0.011207);
  (*data)["Fy"].push_back(0.145173);

  BalanceParameters balanceParameters;
  balanceParameters.rawData_ = data;

  balanceParameters.validateData();
  ASSERT_TRUE(balanceParameters.isValid());

  // Missing Fy column.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);

  balanceParameters;
  balanceParameters.rawData_ = data;

  balanceParameters.validateData();
  ASSERT_FALSE(balanceParameters.isValid());

  // Unequal column length.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(0.145173);

  balanceParameters;
  balanceParameters.rawData_ = data;

  balanceParameters.validateData();
  ASSERT_FALSE(balanceParameters.isValid());
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, constructor) {
  // Realistic data.
  auto data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  // some code duplication from the tests above ...
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(-0.011207);
  (*data)["Fy"].push_back(0.145173);

  BalanceParameters balanceParameters(data);
  ASSERT_TRUE(balanceParameters.isValid());
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceX(), 0.0317253);
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceY(), 0.0317253);
  ASSERT_FLOAT_EQ(balanceParameters.getStartTime(), 0.0);
  ASSERT_FLOAT_EQ(balanceParameters.getStopTime(), 0.009);
  ASSERT_FLOAT_EQ(balanceParameters.getTimeframe(), 0.009);
}

// ____________________________________________________________________________
TEST(BalanceParametersTest, update) {
  // Regular case.
  BalanceParameters balanceParameters;
  ASSERT_FALSE(balanceParameters.isValid());
  ASSERT_FLOAT_EQ(balanceParameters.getTimeframe(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getStartTime(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getStopTime(), 0);
  ASSERT_EQ(balanceParameters.getNumRows(), 0);

  auto data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  // some code duplication from the tests above ...
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(-0.011207);
  (*data)["Fy"].push_back(0.145173);

  balanceParameters.update(data);

  ASSERT_TRUE(balanceParameters.isValid());
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceX(), 0.0317253);
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceY(), 0.0317253);
  ASSERT_FLOAT_EQ(balanceParameters.getStartTime(), 0.0);
  ASSERT_FLOAT_EQ(balanceParameters.getStopTime(), 0.009);
  ASSERT_FLOAT_EQ(balanceParameters.getTimeframe(), 0.009);

  // Unequal column length.
  data =
      std::make_shared<std::unordered_map<std::string, std::vector<float>>>();
  (*data)["abs time (s)"].push_back(0.0);
  (*data)["abs time (s)"].push_back(0.001);
  (*data)["abs time (s)"].push_back(0.002);
  (*data)["abs time (s)"].push_back(0.003);
  (*data)["abs time (s)"].push_back(0.004);
  (*data)["abs time (s)"].push_back(0.005);
  (*data)["abs time (s)"].push_back(0.006);
  (*data)["abs time (s)"].push_back(0.007);
  (*data)["abs time (s)"].push_back(0.008);
  (*data)["abs time (s)"].push_back(0.009);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011368);
  (*data)["Fx"].push_back(0.027848);
  (*data)["Fx"].push_back(0.145133);
  (*data)["Fx"].push_back(-0.011408);
  (*data)["Fx"].push_back(0.066983);
  (*data)["Fx"].push_back(-0.050422);
  (*data)["Fx"].push_back(-0.128612);
  (*data)["Fx"].push_back(-0.011207);
  (*data)["Fx"].push_back(0.145173);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011368);
  (*data)["Fy"].push_back(0.027848);
  (*data)["Fy"].push_back(0.145133);
  (*data)["Fy"].push_back(-0.011408);
  (*data)["Fy"].push_back(0.066983);
  (*data)["Fy"].push_back(-0.050422);
  (*data)["Fy"].push_back(-0.128612);
  (*data)["Fy"].push_back(0.145173);

  balanceParameters.update(data);

  ASSERT_FALSE(balanceParameters.isValid());
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceX(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getMeanForceY(), 0);
  ASSERT_FLOAT_EQ(balanceParameters.getStartTime(), 0.0);
  ASSERT_FLOAT_EQ(balanceParameters.getStopTime(), 0.0);
  ASSERT_FLOAT_EQ(balanceParameters.getTimeframe(), 0.0);
}

// ____________________________________________________________________________
TEST(DataModelTest, defaultConstructor) {
  DataModel dataModel;

  ASSERT_STREQ(dataModel.fileName_.c_str(), "");
  ASSERT_EQ(dataModel.configTimeframe_, 0);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
}

// ____________________________________________________________________________
TEST(DataModelTest, onStartProcessing) {
  // Invalid file.
  DataModel dataModel;

  ASSERT_STREQ(dataModel.fileName_.c_str(), "");
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.0);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_FALSE(dataModel.running_);

  dataModel.onStartProcessing("", 0.05);

  // ...still not running.
  ASSERT_STREQ(dataModel.fileName_.c_str(), "");
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_FALSE(dataModel.running_);

  // Regular case.
  dataModel.onStartProcessing("example_data/KistlerCSV_stub.txt", 0.05);

  ASSERT_STREQ(dataModel.fileName_.c_str(), "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_TRUE(dataModel.running_);

  // Calling it twice should not change anything.
  dataModel.onStartProcessing("example_data/KistlerCSV_stub.txt", 0.05);

  ASSERT_STREQ(dataModel.fileName_.c_str(), "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_TRUE(dataModel.running_);
}

// ____________________________________________________________________________
TEST(DataModelTest, onStopProcessing) {
  DataModel dataModel;

  // Regular case.
  dataModel.onStartProcessing("example_data/KistlerCSV_stub.txt", 0.05);
  ASSERT_TRUE(dataModel.running_);
  dataModel.onStopProcessing();
  ASSERT_FALSE(dataModel.running_);

  // Double call.
  dataModel.onStopProcessing();
  ASSERT_FALSE(dataModel.running_);

  // Start again.
  dataModel.onStartProcessing("example_data/KistlerCSV_stub.txt", 0.05);
  ASSERT_TRUE(dataModel.running_);
}

// ____________________________________________________________________________
TEST(DataModelTest, process) {
  DataModel dataModel;

  // Regular case.
  dataModel.onStartProcessing("example_data/KistlerCSV_large.txt", 0.05);
  ASSERT_TRUE(dataModel.running_);
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);

  dataModel.process();

  ASSERT_FLOAT_EQ(dataModel.timeframe_, 0.05);
  ASSERT_FLOAT_EQ(dataModel.startTime_, 0);
  ASSERT_FLOAT_EQ(dataModel.stopTime_, 0.05);
  ASSERT_EQ(dataModel.firstRow_, PLAYBACK_DELAY_MS);
  ASSERT_EQ(dataModel.lastRow_, PLAYBACK_DELAY_MS + 51);
  ASSERT_EQ(dataModel.numRows_, 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStartTime(), 0);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStopTime(), 0.05);
  ASSERT_EQ(dataModel.balanceParameters_.getNumRows(), 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getTimeframe(), 0.05);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceX(),
                  0.035464098039215686);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceY(),
                  -0.02020123529411765);

  dataModel.process();

  ASSERT_FLOAT_EQ(dataModel.timeframe_, 0.05);
  ASSERT_FLOAT_EQ(dataModel.startTime_, 0.01);
  ASSERT_FLOAT_EQ(dataModel.stopTime_, 0.06);
  ASSERT_EQ(dataModel.firstRow_, 2 * PLAYBACK_DELAY_MS);
  ASSERT_EQ(dataModel.lastRow_, 2 * PLAYBACK_DELAY_MS + 51);
  ASSERT_EQ(dataModel.numRows_, 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStartTime(), 0.01);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStopTime(), 0.06);
  ASSERT_EQ(dataModel.balanceParameters_.getNumRows(), 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getTimeframe(), 0.05);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceX(), 0.037762236);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceY(), -0.0048777051);

  // Corrupt data file.
  dataModel.onResetModel();

  dataModel.onStartProcessing("example_data/KistlerCSV_corrupt.txt", 0.05);
  ASSERT_TRUE(dataModel.running_);
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);

  // should not crash, but emit a corruptFileSignal.
  // can't really test the signals ...
}

// ____________________________________________________________________________
TEST(DataModelTest, onResetModel) {
  DataModel dataModel;

  // Regular case.
  dataModel.onStartProcessing("example_data/KistlerCSV_stub.txt", 0.05);

  ASSERT_STREQ(dataModel.fileName_.c_str(), "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_TRUE(dataModel.running_);

  dataModel.onStopProcessing();
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_FALSE(dataModel.running_);

  dataModel.onResetModel();

  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_FALSE(dataModel.running_);

  // After a single process() shot.
  dataModel.onStartProcessing("example_data/KistlerCSV_large.txt", 0.05);
  ASSERT_TRUE(dataModel.running_);
  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);

  dataModel.process();

  ASSERT_FLOAT_EQ(dataModel.timeframe_, 0.05);
  ASSERT_FLOAT_EQ(dataModel.startTime_, 0);
  ASSERT_FLOAT_EQ(dataModel.stopTime_, 0.05);
  ASSERT_EQ(dataModel.firstRow_, PLAYBACK_DELAY_MS);
  ASSERT_EQ(dataModel.lastRow_, PLAYBACK_DELAY_MS + 51);
  ASSERT_EQ(dataModel.numRows_, 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStartTime(), 0);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getStopTime(), 0.05);
  ASSERT_EQ(dataModel.balanceParameters_.getNumRows(), 51);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getTimeframe(), 0.05);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceX(),
                  0.035464098039215686);
  ASSERT_FLOAT_EQ(dataModel.balanceParameters_.getMeanForceY(),
                  -0.02020123529411765);

  dataModel.onResetModel();

  ASSERT_FLOAT_EQ(dataModel.configTimeframe_, 0.05);
  ASSERT_EQ(dataModel.timeframe_, 0);
  ASSERT_EQ(dataModel.startTime_, 0);
  ASSERT_EQ(dataModel.stopTime_, 0);
  ASSERT_EQ(dataModel.firstRow_, 0);
  ASSERT_EQ(dataModel.lastRow_, 0);
  ASSERT_EQ(dataModel.numRows_, 0);
  ASSERT_FALSE(dataModel.running_);
}

// ____________________________________________________________________________
TEST(ForcePlateFeedbackTest, validateConfigOptions) {
  // Empty file name.
  ASSERT_FALSE(ForcePlateFeedback::validateConfigOptions("", 50.0));

  // Negative timeframe
  ASSERT_FALSE(
      ForcePlateFeedback::validateConfigOptions("/tmp/file.txt", -50.0));

  // Zero timeframe
  ASSERT_FALSE(ForcePlateFeedback::validateConfigOptions("/tmp/file.txt", 0.0));

  // Empty file name and zero timeframe.
  ASSERT_FALSE(ForcePlateFeedback::validateConfigOptions("", 0.0));

  // All good.
  ASSERT_TRUE(ForcePlateFeedback::validateConfigOptions("/tmp/file.txt", 50.0));
}

// ____________________________________________________________________________
TEST(ForcePlateFeedbackTest, combinedTest) {
  // This is a combined test of the constructor, startLiveView, stopLiveView,
  // onStartButtonPressed, onReachedEOF, onInvalidFile, onCorruptFile.
  // The creation of QtWidgets necessitates a QApplication. Initializing a new
  // QApplication in a separate tests gave me memory leaks and I could not find
  // out how to solve it.

  int argc = 0;
  char **argv = new char *[1];

  QApplication app(argc, argv);

  // TEST: constructor

  ForcePlateFeedback forcePlateFeedback;
  // Dependency injection: mock message handler
  delete forcePlateFeedback.messageHandler_;
  forcePlateFeedback.messageHandler_ = new MockMessageHandler();

  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(), "");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0);
  ASSERT_FALSE(forcePlateFeedback.dataModel_->isRunning());
  ASSERT_TRUE(forcePlateFeedback.configWindow_->isEnabled());
  ASSERT_TRUE(forcePlateFeedback.outputWindow_->isEnabled());

  // TEST: startLiveView and stopLiveView

  // Regular case.
  forcePlateFeedback.startLiveView("example_data/KistlerCSV_stub.txt", "50");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Already running.
  forcePlateFeedback.startLiveView("example_data/KistlerCSV_stub.txt", "50");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Stopping.
  forcePlateFeedback.stopLiveView();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Stopping twice.
  forcePlateFeedback.stopLiveView();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Case with invalid config options.

  forcePlateFeedback.startLiveView("", "50");
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Errors in conversion of timeframe to float should not happen because
  // the QLineEdit has a validator.

  // TEST: onStartButtonPressed. This is basically a wrapper for startLiveView
  // and stopLiveView depending on the current state (toggling).

  // Regular case.

  forcePlateFeedback.onStartButtonPressed("example_data/KistlerCSV_stub.txt",
                                          "50");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Another click should stop it.

  forcePlateFeedback.onStartButtonPressed("example_data/KistlerCSV_stub.txt",
                                          "50");
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Another click should start it again.
  forcePlateFeedback.onStartButtonPressed("example_data/KistlerCSV_large.txt",
                                          "13");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_large.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.013);

  // TEST: onReachedEOF
  // This is basically a wrapper for stopLiveView.

  // Regular case.
  forcePlateFeedback.onReachedEOF();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_large.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.013);

  // Double stop.
  forcePlateFeedback.onReachedEOF();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_large.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.013);

  // TEST: onInvalidFile
  // This is basically a wrapper for stopLiveView.

  forcePlateFeedback.startLiveView("example_data/KistlerCSV_stub.txt", "50");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  forcePlateFeedback.onInvalidFile();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Double stop.

  forcePlateFeedback.onInvalidFile();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // TEST: onCorruptFile
  // This is basically a wrapper for stopLiveView.

  forcePlateFeedback.startLiveView("example_data/KistlerCSV_stub.txt", "50");
  ASSERT_TRUE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  forcePlateFeedback.onCorruptFile();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  // Double stop.

  forcePlateFeedback.onCorruptFile();
  ASSERT_FALSE(forcePlateFeedback.running_);
  ASSERT_STREQ(forcePlateFeedback.fileName_.c_str(),
               "example_data/KistlerCSV_stub.txt");
  ASSERT_FLOAT_EQ(forcePlateFeedback.timeframe_, 0.05);

  delete[] argv;
}

// ____________________________________________________________________________
int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}