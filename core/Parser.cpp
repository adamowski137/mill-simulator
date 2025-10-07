#include "Parser.hpp"
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
  m.x = parseFloat('X');
  m.y = parseFloat('Y');
  m.z = parseFloat('Z');

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
