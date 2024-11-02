#include <array>
#include <exception>
#include <format>
#include <fstream>
#include <map>

#include "Drawing.h"

/******************************************** SVG
 **************************************************/

/*

Priklad SVG:

<svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
    <line x1="0" y1="80" x2="100" y2="20" stroke="black" />
</svg>

*/
const std::map<IDrawing::Color, const std::string> color_to_str = {
    {IDrawing::Color::Black, "black"},
    {IDrawing::Color::Blue, "blue"},
    {IDrawing::Color::Green, "green"},
    {IDrawing::Color::Red, "red"},
    {IDrawing::Color::Yellow, "yellow"}};

const std::map<IDrawing::Color, std::array<unsigned char, 3>> color_to_byte = {
    {IDrawing::Color::Black, {0, 0, 0}},
    {IDrawing::Color::Blue, {0, 0, 255}},
    {IDrawing::Color::Green, {0, 255, 0}},
    {IDrawing::Color::Red, {255, 0, 0}},
    {IDrawing::Color::Yellow, {255, 255, 0}}};

void SVGDrawing::Init(int width, int height) {
  front.push_back(std::format(
      "<svg viewBox=\"0 0 {} {}\" xmlns=\"http://www.w3.org/2000/svg\">\n",
      width, height));
  end.push_back("</svg>");
}

void SVGDrawing::Draw_Line(int fromX, int fromY, int toX, int toY, Color clr) {
  auto pos = color_to_str.find(clr);
  if (pos == color_to_str.end()) {
    // should be impossible
    throw std::exception{};
  }
  std::string color_str = pos->second;

  front.push_back(std::format(
      "<line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\">\n", fromX,
      fromY, toX, toY, color_str));
}

void SVGDrawing::Save_To_File(const std::string &fileBaseName) {
  std::ofstream save_file{fileBaseName, std::ios::out};
  for (auto line : front) {
    save_file.write(line.c_str(), line.length());
  }
}

/******************************************** PPM
 * *************************************************/

/*

Priklad PPM:

P6
100 100
255
<pole bajtu>



pole bajtu je posloupnost binarnich R, G a B slozek za sebou, pixely jdou zleva
doprava, shora dolu

*/

void PPMDrawing::Init(int width, int height) {
  //
}

void PPMDrawing::Draw_Line(int fromX, int fromY, int toX, int toY, Color clr) {
  //
}

void PPMDrawing::Save_To_File(const std::string &fileBaseName) {
  //
}
