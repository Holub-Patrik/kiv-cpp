#include <algorithm>
#include <iostream>
#include <map>
#include <memory>

#include "DataLoader.h"
#include "Drawing.h"

#undef min
#undef max

// mapovani retezcu na hodnoty vyctu
const std::map<const std::string, IDrawing::Color> ColorMap = {
    {"black", IDrawing::Color::Black},
    {"blue", IDrawing::Color::Blue},
    {"green", IDrawing::Color::Green},
    {"red", IDrawing::Color::Red},
    {"yellow", IDrawing::Color::Yellow}};

int main(int argc, char **argv) {
  Data_Loader ldr;

  std::unique_ptr<IDrawing> draw = std::make_unique<SVGDrawing>();

  // nacteni vstupu
  std::cout << "Starting loading ... " << std::endl;
  if (!ldr.Load("sample.csv")) {
    std::cerr << "Nepodarilo se nacist soubor" << std::endl;
    return -1;
  }
  std::cout << "Loaded!" << std::endl;

  // nalezeni leveho horniho a praveho spodniho rohu obrazku
  std::cout << "Starting minmax ... " << std::endl;
  Data_Loader::Line minmax = *ldr.begin();
  std::for_each(
      ldr.begin(), ldr.end(), [&minmax](const Data_Loader::Line &line) {
        minmax.x1 = std::min(std::min(line.x1, line.x2), minmax.x1);
        minmax.x2 = std::max(std::max(line.x1, line.x2), minmax.x2); // x1
        minmax.y1 = std::min(std::min(line.y1, line.y2), minmax.y1);
        minmax.y2 = std::max(std::max(line.y1, line.y2), minmax.y2); // y1
      });
  std::cout << "Done!" << std::endl;

  // inicializace kreslitka
  std::cout << "Initiating ... " << std::endl;
  draw->Init(minmax.x2 - minmax.x1, minmax.y2 - minmax.y1);
  std::cout << "Done!" << std::endl;

  // lambda pro nalezeni mapovani barvy
  auto mapColor = [](const std::string &colStr) -> IDrawing::Color {
    auto itr = ColorMap.find(colStr);
    if (itr == ColorMap.end())
      return IDrawing::Color::Black; // unknown
    return itr->second;
  };

  // vykresleni car
  for (auto &line : ldr)
    draw->Draw_Line(line.x1, line.y1, line.x2, line.y2, mapColor(line.color));

  // ulozeni vystupu
  std::cout << "Saving!" << std::endl;
  draw->Save_To_File("output");

  return 0;
}
