// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./KistlerFile.h"

// ____________________________________________________________________________
KistlerCSVFile::KistlerCSVFile(const std::string &fileName)
    : KistlerFile(fileName) {
  // Sanity checks on the provided file.
  validateFile();

  // Read column headers so we know which data we have available.
  parseColumnNames();

  // Now we're ready for getting data
}

// ____________________________________________________________________________
void KistlerCSVFile::validateFile() {
  std::ifstream file(fileName_);

  // makes sense?
  isValid_ = true;

  // (1) Check if file exists.
  if (!file) {
    isValid_ = false;
    std::cerr << "Error in KistlerCSVFile::validateFile(): No such file or "
                 "directory: "
              << fileName_ << std::endl;
    return;
  }

  // (1) Check if file is non-empty.
  // Thanks https://stackoverflow.com/a/2390938
  if (file.peek() == std::ifstream::traits_type::eof()) {
    isValid_ = false;
    std::cerr << "Error in KistlerCSVFile::validateFile(): File is empty: "
              << fileName_ << std::endl;
    return;
  }

  // (2) Check if there is "BioWare" in the first line.
  std::string line;
  std::getline(file, line);

  // Thanks https://stackoverflow.com/a/2340309
  if (line.find("BioWare") == std::string::npos) {
    isValid_ = false;
    std::cerr << "Error in KistlerCSVFile::validateFile(): File does not "
                 "appear to be a valid BioWare file: "
              << fileName_ << std::endl;
    return;
  }

  // (3) Check if there are sensible column headers in line 18 (the variable
  // names, like "Fx").
  for (int i = 0; i < 17; i++) {
    std::getline(file, line);
  }

  // For now this is hard-coded, but this might change (e.g. depending on
  // BioWare settings).
  if (line.find("abs time") == std::string::npos) {
    isValid_ = false;
    std::cerr << "Error in KistlerCSVFile::validateFile(): File does not "
                 "appear to be a valid BioWare file: "
              << fileName_ << std::endl;
    return;
  }
}

// ____________________________________________________________________________
void KistlerCSVFile::parseColumnNames() {
  std::ifstream file(fileName_);

  if (!isValid_) {
    std::cerr << "Error in KistlerCSVFile::parseColumnNames(): File does not "
                 "appear to be a valid BioWare file: "
              << fileName_ << std::endl;
  }

  std::string line;
  std::getline(file, line);

  // Column headers are in line 18.
  for (int i = 0; i < 17; i++) {
    std::getline(file, line);
  }

  columnNames_ = sliceRow(line, '\t'); // hard-coded delimiter ...

  numCols_ = columnNames_.size();
}

// ____________________________________________________________________________
std::vector<std::string> KistlerCSVFile::sliceRow(std::string line,
                                                  char delimiter) {
  // Thanks https://stackoverflow.com/a/55263720
  std::string tmp;
  std::stringstream ss(line);
  std::vector<std::string> elements;

  while (getline(ss, tmp, delimiter)) {
    elements.push_back(tmp);
  }

  // If the line ends with a delimiter it should append an empty element.
  if (!line.empty() && line.back() == delimiter) {
    elements.push_back("");
  }

  return elements;
}

// ____________________________________________________________________________
std::vector<float>
KistlerCSVFile::stringToFloatVector(std::vector<std::string> stringVector) {
  std::vector<float> floatVector;

  if (stringVector.empty())
    return floatVector;

  // Avoids multiple allocations, thanks ChatGPT!
  floatVector.reserve(stringVector.size());

  for (auto &element : stringVector) {
    try {
      float value = std::stof(element);
      floatVector.push_back(value);
    } catch (std::exception &e) {
      std::cerr << "Error in KistlerCSVFile::stringToFloatVector(): Cannot "
                   "convert string to float. Seems like the data is corrupt."
                << std::endl;
      exit(EXIT_FAILURE); // TODO: proper error dialogs...
    }
  }

  return floatVector;
}

// ____________________________________________________________________________
std::unordered_map<std::string, std::vector<float>>
KistlerCSVFile::getData(int startRow, int stopRow) const {
  // Method to extract raw data from the file by row indices.
  // startRow and stopRow specify which rows to return from the file
  // (zero-based indexing).
  // Use negative indices for retrieving all data, e.g. startRow = -1
  // and stopRow = -1 will return all data; startRow = 26 and stopRow = -1
  // will return data from row 27 until the end of the file.
  // It returns a map, so that the data columns can be accessed by their
  // column names, e.g. "Fx"

  // Check for invalid row indices.
  if (stopRow < startRow && stopRow != -1) {
    std::cerr << "Error in KistlerCSVFile::getData(): Invalid row indices "
                 "startRow and/or stopRow. This can happen e.g. if startRow > "
                 "stopRow."
              << std::endl;
    exit(EXIT_FAILURE); // replace with exception handling
  }

  std::unordered_map<std::string, std::vector<float>> data;

  for (const auto &column : columnNames_) {
    data[column] = std::vector<float>();
  }

  // We can reserve some memory in advance to avoid multiple allocations.
  if (startRow != -1 && stopRow != -1) {
    for (auto &column : data) {
      column.second.reserve(stopRow - startRow + 1);
    }
  }

  // rest of the logic
}

// ____________________________________________________________________________
void KistlerDatFile::validateFile() {}
