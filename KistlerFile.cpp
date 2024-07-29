// Copyright 2024
// Author: Paul Soelder <p.soelder@mailbox.org>

#include "./KistlerFile.h"

// ____________________________________________________________________________
KistlerCSVFile::KistlerCSVFile(const std::string &fileName)
    : KistlerFile(fileName) {
  // Sanity checks on the provided file.
  validateFile();

  // Read column headers so we know which data we have available.
  parseMetaData();

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
void KistlerCSVFile::parseMetaData() {
  // Get the sample rate.

  // Get the column headers.
  std::ifstream file(fileName_);

  if (!isValid_) {
    std::cerr << "Error in KistlerCSVFile::parseMetaData(): File does not "
                 "appear to be a valid BioWare file: "
              << fileName_ << std::endl;
    return;
  }

  std::string line;
  std::getline(file, line);

  // Sampling rates are in line 4.
  for (int i = 0; i < 3; i++) {
    std::getline(file, line);
  }

  std::vector<std::string> samplingRates_ =
      sliceRow(line, '\t'); // hard-coded delimiter ...

  if (!(samplingRates_[0] == "Rate (Hz):")) {
    std::cerr << "Error in KistlerCSVFile::parseMetaData(): Could not "
                 "determine the sampling rate of the file, it does not appear "
                 "to be a valid BioWare file: "
              << fileName_ << std::endl;
    return;
  } else {
    // Assuming that the sampling rate is the same for each channel.
    if (samplingRates_.size() > 1) {
      samplingRate_ = std::stof(samplingRates_[1]);

      if (samplingRate_ == 0) {
        std::cerr << "Error in KistlerCSVFile::parseMetaData(): Determined a "
                     "sampling rate of the file of 0 Hz, it does not appear to "
                     "be a valid BioWare file: "
                  << fileName_ << std::endl;
      }
      qDebug() << "Detected sampling rate of " << samplingRate_ << "Hz.";

    } else {
      std::cerr
          << "Error in KistlerCSVFile::parseMetaData(): Could not "
             "determine the sampling rate of the file, it does not appear "
             "to be a valid BioWare file: "
          << fileName_ << std::endl;
      return;
    }
  }

  // Column headers are in line 18 (4 already read for the sampling rate).
  for (int i = 0; i < 14; i++) {
    std::getline(file, line);
  }

  columnNames_ = sliceRow(line, '\t'); // hard-coded delimiter ...

  numCols_ = columnNames_.size();
}

// ____________________________________________________________________________
std::vector<std::string> KistlerCSVFile::sliceRow(std::string line,
                                                  char delimiter) {
  // Remove newline and carriage return.
  line.erase(std::remove(line.begin(), line.end(), '\n'), line.cend());
  line.erase(std::remove(line.begin(), line.end(), '\r'), line.cend());

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

  std::ifstream file(fileName_);
  std::string line;
  std::getline(file, line);

  // Data starts at line 20.
  for (int i = 0; i < 18; i++) {
    std::getline(file, line);
  }

  // Take care of startRow.
  if (startRow != -1) {
    for (int i = 0; i < startRow; i++) {
      std::getline(file, line);
    }
  }

  // Note: This is a big performance limitation, as the number of I/O operations
  // increase linearly with application runtime. We will have to think about
  // buffering or using byte-wise access with istream::seekg().

  // Take care of stopRow.
  int nRows;
  if (stopRow != -1 && startRow != -1) {
    nRows = stopRow - startRow + 1;
  } else if (stopRow != -1 && startRow == -1) {
    nRows = stopRow + 1;
  } else {
    nRows = -1; // indicator to read until EOF in the loop below.
  }

  // We can reserve some memory in advance to avoid multiple allocations.
  if (nRows != -1) {
    for (auto &column : data) {
      column.second.reserve(nRows);
    }
  }

  // Read nRows lines.
  bool done = false;
  int i = 0;
  do {
    // Continue if we have not reached nRows yet OR we should read until EOF
    // (nRows == -1).
    if ((nRows != -1 && i < nRows) || nRows == -1) {
      if (!std::getline(file, line)) {
        qDebug() << "KistlerCSVFile::getData(int, int): reached EOF";
        // reached EOF.
        done = true;
        break;
      }

      auto row = sliceRow(line, '\t');

      for (size_t j = 0; j < columnNames_.size(); j++) {
        try {
          float value = std::stof(row[j]);
          data[columnNames_[j]].push_back(value);
        } catch (std::exception &e) {
          std::cerr
              << "Error in KistlerCSVFile::stringToFloatVector(): Cannot "
                 "convert string to float. Seems like the data is corrupt."
              << std::endl;
          exit(EXIT_FAILURE); // TODO: proper error dialogs...
        }
      }
      i++;
    } else
      done = true;
  } while (!done);

  return data;
}

// ____________________________________________________________________________
void KistlerDatFile::validateFile() {}