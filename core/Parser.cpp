#include "Parser.hpp"
#include "models/Blade.hpp"
#include <charconv>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

Move Parser::ParseLine(std::string_view line) {
  auto parseFloat = [&](char prefix) -> std::optional<float> {
    if (auto pos = line.find(prefix); pos != std::string_view::npos) {
      auto start = pos + 1;
      auto end = line.find_first_not_of("0123456789.-", start);
      float value = 0.0f;
      std::from_chars(line.data() + start,
                      line.data() +
                          (end == std::string_view::npos ? line.size() : end),
                      value);
      return value;
    }
    return std::nullopt;
  };
  Move m;
  // In the file Z is the vertical move whereas in our program is the Y
  // coordinate
  m.x = parseFloat('X');
  m.z = parseFloat('Y');
  m.y = parseFloat('Z');

  return m;
}

std::vector<Move> Parser::ParseGCode(std::string gcode) {
  auto lines = gcode | std::views::split('\n') |
               std::views::transform([](auto &&r) {
                 return std::string_view(&*r.begin(), std::ranges::distance(r));
               }) |
               std::views::filter(
                   [](std::string_view line) { return line.contains("G01"); }) |
               std::views::transform(ParseLine);
  return std::vector<Move>(lines.begin(), lines.end());
}

std::pair<BladeType, float> Parser::ParseFileName(const std::string &name) {
  return {name[2] == 'f' ? BladeType::FLAT : BladeType::BALL,
          std::stoi(name.substr(3))};
}
