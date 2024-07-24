// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./ForcePlateFeedback.h"
#include <gtest/gtest.h>

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

// sliceRow()

// parseColumnNames()

// stringToFloatVector()

// ____________________________________________________________________________

// bool ForcePlateFeedback::validateConfigOptions()

// ____________________________________________________________________________

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}