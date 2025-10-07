#pragma once

#include "Move.hpp"
#include "Vector.hpp"
#include "models/Blade.hpp"
#include <list>
#include <vector>
class Simulator {
public:
  Simulator(Blade &blade);
  void setMoves(const std::vector<Move> &moves);
  void simulate(float speed);

private:
  Blade &m_blade;
  std::list<math137::Vector3f> m_postions;
  static constexpr float c_stepSize = 0.001f;
  static constexpr float c_scale = 10.f;
};
