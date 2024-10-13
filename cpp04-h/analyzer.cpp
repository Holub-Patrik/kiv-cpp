#include "analyzer.h"

#include <exception>
#include <iostream>

#include <algorithm>
#include <fstream>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <vector>

class AnalyzerException : public std::exception {
  const char *msg;

public:
  AnalyzerException(const char *msg) : msg(msg) {}
  const char *what() const noexcept override { return msg; }
};

CAnalyzer::CAnalyzer(std::vector<double> &&numbers) noexcept
    : mNumbers(numbers) {
  // NOTE: v teto metode nic nemente!
}

CAnalyzer CAnalyzer::Create(const std::string &fileName) {
  // NOTE: v teto metode nic nemente!

  std::vector<double> numbers;

  // otevre soubor
  std::ifstream ifs(fileName, std::ios::in | std::ios::binary);

  if (!ifs.is_open()) {
    throw std::invalid_argument{"Nelze otevrit soubor " + fileName};
  }

  // zjisti velikost a pole cisel zvetsi, aby se tam cisla vesla
  ifs.seekg(0, std::ios::end);
  numbers.resize(ifs.tellg() / sizeof(double));
  ifs.seekg(0, std::ios::beg);

  // precte obsah souboru
  ifs.read(reinterpret_cast<char *>(numbers.data()),
           numbers.size() * sizeof(double));

  // overi, ze se precetlo vsechno
  if (ifs.gcount() != numbers.size() * sizeof(double)) {
    throw std::runtime_error{"Nelze precist cisla ze souboru " + fileName};
  }

  return CAnalyzer(std::move(numbers));
}

void CAnalyzer::Cache_Chunk_Averages() {
  for (const auto chunk :
       mNumbers | std::views::chunk(Measurements_Per_Chunk)) {
    averages.push_back(std::accumulate(chunk.begin(), chunk.end(), 0) /
                       (double)Measurements_Per_Chunk);
  }
}

double CAnalyzer::Get_Min_Chunk_Average() const {
  if (averages.empty()) {
    throw AnalyzerException("Acessed min before caching chunk averages");
  }
  return *std::ranges::min_element(averages.begin(), averages.end());
}

double CAnalyzer::Get_Max_Chunk_Average() const {
  if (averages.empty()) {
    throw AnalyzerException("Acessed min before caching chunk averages");
  }
  return *std::ranges::max_element(averages.begin(), averages.end());
}

std::array<double, CAnalyzer::Measurements_Per_Chunk>
CAnalyzer::Get_Piecewise_Averages() {
  if (averages.empty()) {
    throw AnalyzerException("Acessed min before caching chunk averages");
  }
  /*
   * Implementacni poznamka:
   * pomoci algoritmu, ranges a views se pokuste vypocitat prumery pres sloupce
   * hodnot, tj. jestlize se nachazi v souboru hodnoty po 10 cislech, vypoctete
   * 10 prumeru napr. v prvnim prumeru budou zahrnuty indexy prvku 0, 10, 20,
   * 30, ... ve druhem indexy prvku 1, 11, 21, 31, ...
   */
  std::array<double, Measurements_Per_Chunk> vals;
  auto arr_iter = vals.begin();

  for (const int i : std::views::iota(0, (int)Measurements_Per_Chunk)) {

    auto cols = std::ranges::subrange(mNumbers.begin() + i, mNumbers.end()) |
                std::views::stride(Measurements_Per_Chunk);

    *arr_iter++ =
        std::accumulate(cols.begin(), cols.end(), 0) / (double)cols.size();
  }

  return vals;
}
