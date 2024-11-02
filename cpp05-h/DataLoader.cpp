#include <fstream>
#include <sstream>
#include <string>

#include "DataLoader.h"

bool Data_Loader::Load(const std::string &file) {
  // Hlavicka: x1,y1,x2,y2,color
  // cisla jsou integery, color je retezec (konkretne jeden z vyctu: black,
  // blue, green, yellow, red)

  std::ifstream fs(file, std::ios::in);
  std::string str_line;
  std::string part;
  std::getline(fs, str_line);

  while (std::getline(fs, str_line)) {
    std::stringstream ss(str_line);
    Line line;
    int *variables[4] = {&line.x1, &line.y1, &line.x2, &line.y2};
    for (int i = 0; i < 4; i++) {
      std::getline(ss, part, ',');
      *variables[i] = stoi(part);
    }
    std::getline(ss, part, ',');
    line.color = part;
  }

  return true;
}

std::list<Data_Loader::Line>::const_iterator Data_Loader::begin() const {
  return mLines.begin();
}

std::list<Data_Loader::Line>::const_iterator Data_Loader::end() const {
  return mLines.end();
}
