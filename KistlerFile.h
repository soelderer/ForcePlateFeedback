// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#pragma once

#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

// Abstract class for representing input data files.
// There are two file formats: a CSV-style plain-text format, and a binary
// encoded ".dat" format. The input data files store information from the
// sensors of the force plate:
// - The absolute time since beginning of measurements (in seconds)
// - Forces in every direction (Fx, Fy, Fz in Newton)
// - Moments in every direction (Mx, My, Mz in Newton meters)
// - Force application point "COP" (Ax, Ay as coordinates in meters)
// An example of a CSV file is in KisterCSV_example.txt
// The class provides methods to read data from the files.
class KistlerFile {
public:
  // The constructor takes a file name as input and performs some sanity
  // checks (see below).
  KistlerFile() {}
  KistlerFile(const std::string &fileName) : fileName_(fileName) {}
  virtual ~KistlerFile() {}

  // Method for some sanity checks on the file:
  // Does the file type match the subclass, is there the right magic number,
  // is the data in proper format etc. (see subclasses for details).
  // The method sets the isValid_ member variable, and will throw
  // exceptions if this makes sense in a later stage of the project.
  virtual void validateFile() = 0;

  // Method to extract raw data from the file by row indices.
  // startRow and stopRow specify which rows to return from the file
  // (zero-based indexing).
  // Use negative indices for retrieving all data, e.g. startRow = -1
  // and stopRow = -1 will return all data; startRow = 26 and stopRow = -1
  // will return data from row 27 until the end of the file.
  // It returns a map, so that the data columns can be accessed by their
  // column names, e.g. "Fx"
  virtual std::unordered_map<std::string, std::vector<float>>
  getData(int startRow = -1, int stopRow = -1) const = 0;

  // Method to extract raw data from the file by absolute timestamps.
  // Use negative timestamps for retreiving all data, e.g. startTime = -1
  // and stopTime = -1 will return all data; startTime = 0.04, stopTime = -1
  // will return data from 40ms until the end of the file.
  // It returns a map, so that the data columns can be accessed by their
  // column names, e.g. "Fx"
  virtual std::unordered_map<std::string, std::vector<float>>
  getData(float startTime = -1, float stopTime = -1) const = 0;

  std::string getFilename() const { return fileName_; }

  bool isValid() const { return isValid_; }

protected:
  std::string fileName_;
  bool isValid_;
};

// Subclass to represent CSV files with raw data.
class KistlerCSVFile : public KistlerFile {
public:
  // CSV-specific implementation of the constructor.
  KistlerCSVFile() {}
  KistlerCSVFile(const std::string &fileName);
  ~KistlerCSVFile() {}

  // CSV-specific implementations of sanity checks for the file.
  // This will check:
  // (1) If the file exists and is non-empty.
  // (2) If there is a "BioWare" string in the first line.
  // (3) If there are sensible column headers in line 18 (the variable name)
  // (4) If there are sensible column headers in line 19 (the SI units, e.g.
  //     Newton)
  void validateFile() override;

  // CSV-specific implementations of getData.
  std::unordered_map<std::string, std::vector<float>>
  getData(int startRow = -1, int stopRow = -1) const override;

  std::unordered_map<std::string, std::vector<float>>
  getData(float startTime = -1, float stopTime = -1) const override {}

  // Parse the CSV header to get the column names.
  void parseColumnNames();

  // Slice a single CSV row into separate strings by a given delimiter.
  static std::vector<std::string> sliceRow(std::string line_, char delimiter);

  // Convert a vector of strings into a vector of floats.
  static std::vector<float>
  stringToFloatVector(std::vector<std::string> stringVector);

  FRIEND_TEST(KistlerFileTest, KistlerCSVFileConstructor);

private:
  // Column/variable names of the file.
  std::vector<std::string> columnNames_;

  // The number of columns in the file.
  int numCols_;
};

// Subclass to represent binary .dat files with raw data.
class KistlerDatFile : public KistlerFile {
public:
  // .dat-specific implementation of the constructor.
  KistlerDatFile() {}
  KistlerDatFile(const std::string &fileName) : KistlerFile(fileName) {}
  ~KistlerDatFile() {}

  // .dat-specific implementations of sanity checks for the file.
  void validateFile() override;

  // .dat-specific implementations of getData.
  std::unordered_map<std::string, std::vector<float>>
  getData(int startRow = -1, int stopRow = -1) const override {}

  std::unordered_map<std::string, std::vector<float>>
  getData(float startTime = -1, float stopTime = -1) const override {}
};