#pragma once
#include "Move.hpp"
#include "models/Blade.hpp"
#include <string>
#include <string_view>
#include <vector>

class Parser {
public:
  static std::vector<Move> ParseGCode(std::string gcode);
  static std::pair<BladeType, float> ParseFileName(const std::string &filename);

private:
  static Move ParseLine(std::string_view line);
};
