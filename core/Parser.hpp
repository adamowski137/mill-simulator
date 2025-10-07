#pragma once
#include "Move.hpp"
#include <string>
#include <string_view>
#include <vector>

class Parser {
public:
  static std::vector<Move> ParseGCode(std::string gcode);

private:
  static Move ParseLine(std::string_view line);
};
