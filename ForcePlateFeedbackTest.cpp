// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./ForcePlateFeedback.h"
#include <gtest/gtest.h>

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
TEST(KistlerCSVFileTest, parseColumnNames) {
  // Regular case.
  KistlerCSVFile kistlerFile("example_data/KistlerCSV_example.txt");
  // Constructor calls parseColumnNames(), so let's reset the column names
  // and explicitly call parseColumnNames.
  kistlerFile.columnNames_ = std::vector<std::string>();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 0);
  kistlerFile.parseColumnNames();
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

  // Other column names.
  kistlerFile = KistlerCSVFile("example_data/KistlerCSV_wrong_column.txt");
  kistlerFile.columnNames_ = std::vector<std::string>();
  ASSERT_EQ(kistlerFile.columnNames_.size(), 0);
  kistlerFile.parseColumnNames();
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

  // Edge cases not really expected as this would be a corrupted CSV file.
}

// stringToFloatVector()  -- dont need that anymore?

// ____________________________________________________________________________
TEST(KistlerCSVFileTest, getDataByIndices) {
  KistlerCSVFile kistlerFile("example_data/KistlerCSV_example.txt");
  std::unordered_map<std::string, std::vector<float>> data;

  // Get the first row.
  data = kistlerFile.getData(0, 0);
  ASSERT_EQ(data.size(), 9);
  ASSERT_EQ(data["abs time (s)"].size(), 1);
  ASSERT_EQ(data["Fx"].size(), 1);
  ASSERT_EQ(data["Fy"].size(), 1);
  ASSERT_EQ(data["Fz"].size(), 1);
  ASSERT_EQ(data["Mx"].size(), 1);
  ASSERT_EQ(data["My"].size(), 1);
  ASSERT_EQ(data["Mz"].size(), 1);
  ASSERT_EQ(data["Ax"].size(), 1);
  ASSERT_EQ(data["Ay"].size(), 1);
  ASSERT_FLOAT_EQ(data["abs time (s)"][0], 0);
  ASSERT_FLOAT_EQ(data["Fx"][0], 0.145133);
  ASSERT_FLOAT_EQ(data["Fy"][0], -0.010285);
  ASSERT_FLOAT_EQ(data["Fz"][0], -0.126362);
  ASSERT_FLOAT_EQ(data["Mx"][0], -0.362161);
  ASSERT_FLOAT_EQ(data["My"][0], 0.150046);
  ASSERT_FLOAT_EQ(data["Mz"][0], 0.001693);
  ASSERT_FLOAT_EQ(data["Ax"][0], 0);
  ASSERT_FLOAT_EQ(data["Ay"][0], 0);

  // Get rows 9 to 12.
  data = kistlerFile.getData(8, 11);
  ASSERT_EQ(data.size(), 9);
  ASSERT_EQ(data["abs time (s)"].size(), 4);
  ASSERT_EQ(data["Fx"].size(), 4);
  ASSERT_EQ(data["Fy"].size(), 4);
  ASSERT_EQ(data["Fz"].size(), 4);
  ASSERT_EQ(data["Mx"].size(), 4);
  ASSERT_EQ(data["My"].size(), 4);
  ASSERT_EQ(data["Mz"].size(), 4);
  ASSERT_EQ(data["Ax"].size(), 4);
  ASSERT_EQ(data["Ay"].size(), 4);
  // Row 9
  ASSERT_FLOAT_EQ(data["abs time (s)"][0], 0.008);
  ASSERT_FLOAT_EQ(data["Fx"][0], -0.011207);
  ASSERT_FLOAT_EQ(data["Fy"][0], -0.205451);
  ASSERT_FLOAT_EQ(data["Fz"][0], -1.102404);
  ASSERT_FLOAT_EQ(data["Mx"][0], 0.238696);
  ASSERT_FLOAT_EQ(data["My"][0], 0.348605);
  ASSERT_FLOAT_EQ(data["Mz"][0], -0.067442);
  ASSERT_FLOAT_EQ(data["Ax"][0], 0);
  ASSERT_FLOAT_EQ(data["Ay"][0], 0);
  // Row 10
  ASSERT_FLOAT_EQ(data["abs time (s)"][1], 0.009);
  ASSERT_FLOAT_EQ(data["Fx"][1], 0.145173);
  ASSERT_FLOAT_EQ(data["Fy"][1], -0.049630);
  ASSERT_FLOAT_EQ(data["Fz"][1], -1.428445);
  ASSERT_FLOAT_EQ(data["Mx"][1], 0.147898);
  ASSERT_FLOAT_EQ(data["My"][1], -0.124266);
  ASSERT_FLOAT_EQ(data["Mz"][1], 0.022363);
  ASSERT_FLOAT_EQ(data["Ax"][1], 0);
  ASSERT_FLOAT_EQ(data["Ay"][1], 0);
  // Row 11
  ASSERT_FLOAT_EQ(data["abs time (s)"][2], 0.01);
  ASSERT_FLOAT_EQ(data["Fx"][2], -0.050342);
  ASSERT_FLOAT_EQ(data["Fy"][2], -0.088255);
  ASSERT_FLOAT_EQ(data["Fz"][2], -0.288165);
  ASSERT_FLOAT_EQ(data["Mx"][2], 0.192003);
  ASSERT_FLOAT_EQ(data["My"][2], 0.039073);
  ASSERT_FLOAT_EQ(data["Mz"][2], -0.065504);
  ASSERT_FLOAT_EQ(data["Ax"][2], 0);
  ASSERT_FLOAT_EQ(data["Ay"][2], 0);
  // Row 12
  ASSERT_FLOAT_EQ(data["abs time (s)"][3], 0.011);
  ASSERT_FLOAT_EQ(data["Fx"][3], 0.066863);
  ASSERT_FLOAT_EQ(data["Fy"][3], -0.010165);
  ASSERT_FLOAT_EQ(data["Fz"][3], 0.364267);
  ASSERT_FLOAT_EQ(data["Mx"][3], 0.019169);
  ASSERT_FLOAT_EQ(data["My"][3], 0.249212);
  ASSERT_FLOAT_EQ(data["Mz"][3], 0.005645);
  ASSERT_FLOAT_EQ(data["Ax"][3], -0.684146);
  ASSERT_FLOAT_EQ(data["Ay"][3], 0.052623);

  // Get rows from the beginning to line 2.
  data = kistlerFile.getData(-1, 1);
  ASSERT_EQ(data.size(), 9);
  ASSERT_EQ(data["abs time (s)"].size(), 2);
  ASSERT_EQ(data["Fx"].size(), 2);
  ASSERT_EQ(data["Fy"].size(), 2);
  ASSERT_EQ(data["Fz"].size(), 2);
  ASSERT_EQ(data["Mx"].size(), 2);
  ASSERT_EQ(data["My"].size(), 2);
  ASSERT_EQ(data["Mz"].size(), 2);
  ASSERT_EQ(data["Ax"].size(), 2);
  ASSERT_EQ(data["Ay"].size(), 2);
  // Row 1.
  ASSERT_FLOAT_EQ(data["abs time (s)"][0], 0);
  ASSERT_FLOAT_EQ(data["Fx"][0], 0.145133);
  ASSERT_FLOAT_EQ(data["Fy"][0], -0.010285);
  ASSERT_FLOAT_EQ(data["Fz"][0], -0.126362);
  ASSERT_FLOAT_EQ(data["Mx"][0], -0.362161);
  ASSERT_FLOAT_EQ(data["My"][0], 0.150046);
  ASSERT_FLOAT_EQ(data["Mz"][0], 0.001693);
  ASSERT_FLOAT_EQ(data["Ax"][0], 0);
  ASSERT_FLOAT_EQ(data["Ay"][0], 0);
  // Row 2.
  ASSERT_FLOAT_EQ(data["abs time (s)"][1], 0.001);
  ASSERT_FLOAT_EQ(data["Fx"][1], -0.011368);
  ASSERT_FLOAT_EQ(data["Fy"][1], -0.127600);
  ASSERT_FLOAT_EQ(data["Fz"][1], -1.756052);
  ASSERT_FLOAT_EQ(data["Mx"][1], -0.102902);
  ASSERT_FLOAT_EQ(data["My"][1], -0.404231);
  ASSERT_FLOAT_EQ(data["Mz"][1], 0.046690);
  ASSERT_FLOAT_EQ(data["Ax"][1], 0);
  ASSERT_FLOAT_EQ(data["Ay"][1], 0);

  // Get rows from line 30 to EOF.
  data = kistlerFile.getData(29, -1);
  ASSERT_EQ(data.size(), 9);
  ASSERT_EQ(data["abs time (s)"].size(), 2);
  ASSERT_EQ(data["Fx"].size(), 2);
  ASSERT_EQ(data["Fy"].size(), 2);
  ASSERT_EQ(data["Fz"].size(), 2);
  ASSERT_EQ(data["Mx"].size(), 2);
  ASSERT_EQ(data["My"].size(), 2);
  ASSERT_EQ(data["Mz"].size(), 2);
  ASSERT_EQ(data["Ax"].size(), 2);
  ASSERT_EQ(data["Ay"].size(), 2);
  // Row 30.
  ASSERT_FLOAT_EQ(data["abs time (s)"][0], 0.029);
  ASSERT_FLOAT_EQ(data["Fx"][0], -0.050422);
  ASSERT_FLOAT_EQ(data["Fy"][0], 0.145775);
  ASSERT_FLOAT_EQ(data["Fz"][0], 0.852810);
  ASSERT_FLOAT_EQ(data["Mx"][0], -0.029384);
  ASSERT_FLOAT_EQ(data["My"][0], 0.141809);
  ASSERT_FLOAT_EQ(data["Mz"][0], 0.007898);
  ASSERT_FLOAT_EQ(data["Ax"][0], -0.166285);
  ASSERT_FLOAT_EQ(data["Ay"][0], -0.034456);
  // Row 31.
  ASSERT_FLOAT_EQ(data["abs time (s)"][1], 0.03);
  ASSERT_FLOAT_EQ(data["Fx"][1], -0.011408);
  ASSERT_FLOAT_EQ(data["Fy"][1], -0.205690);
  ASSERT_FLOAT_EQ(data["Fz"][1], -0.125492);
  ASSERT_FLOAT_EQ(data["Mx"][1], 0.069752);
  ASSERT_FLOAT_EQ(data["My"][1], 0.006508);
  ASSERT_FLOAT_EQ(data["Mz"][1], 0.067055);
  ASSERT_FLOAT_EQ(data["Ax"][1], 0);
  ASSERT_FLOAT_EQ(data["Ay"][1], 0);

  // Read a whole file.
  kistlerFile = KistlerCSVFile("example_data/KistlerCSV_stub.txt");
  data = kistlerFile.getData(-1, -1);
  ASSERT_EQ(data.size(), 9);
  ASSERT_EQ(data["abs time (s)"].size(), 2);
  ASSERT_EQ(data["Fx"].size(), 2);
  ASSERT_EQ(data["Fy"].size(), 2);
  ASSERT_EQ(data["Fz"].size(), 2);
  ASSERT_EQ(data["Mx"].size(), 2);
  ASSERT_EQ(data["My"].size(), 2);
  ASSERT_EQ(data["Mz"].size(), 2);
  ASSERT_EQ(data["Ax"].size(), 2);
  ASSERT_EQ(data["Ay"].size(), 2);
  // Row 30.
  ASSERT_FLOAT_EQ(data["abs time (s)"][0], 0.029);
  ASSERT_FLOAT_EQ(data["Fx"][0], -0.050422);
  ASSERT_FLOAT_EQ(data["Fy"][0], 0.145775);
  ASSERT_FLOAT_EQ(data["Fz"][0], 0.852810);
  ASSERT_FLOAT_EQ(data["Mx"][0], -0.029384);
  ASSERT_FLOAT_EQ(data["My"][0], 0.141809);
  ASSERT_FLOAT_EQ(data["Mz"][0], 0.007898);
  ASSERT_FLOAT_EQ(data["Ax"][0], -0.166285);
  ASSERT_FLOAT_EQ(data["Ay"][0], -0.034456);
  // Row 31.
  ASSERT_FLOAT_EQ(data["abs time (s)"][1], 0.03);
  ASSERT_FLOAT_EQ(data["Fx"][1], -0.011408);
  ASSERT_FLOAT_EQ(data["Fy"][1], -0.205690);
  ASSERT_FLOAT_EQ(data["Fz"][1], -0.125492);
  ASSERT_FLOAT_EQ(data["Mx"][1], 0.069752);
  ASSERT_FLOAT_EQ(data["My"][1], 0.006508);
  ASSERT_FLOAT_EQ(data["Mz"][1], 0.067055);
  ASSERT_FLOAT_EQ(data["Ax"][1], 0);
  ASSERT_FLOAT_EQ(data["Ay"][1], 0);
}

// ____________________________________________________________________________

// bool ForcePlateFeedback::validateConfigOptions()

// ____________________________________________________________________________

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}