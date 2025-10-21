#pragma once

#include "Move.hpp"
#include "Vector.hpp"
#include "models/Blade.hpp"
#include "models/Block.hpp"
#include <list>
#include <vector>
class Simulator {
public:
  Simulator(Blade &blade, Block &block);
  void setMoves(const std::vector<Move> &moves);
  void simulate();
  void finish();

  bool m_simulate;
  int m_speed;
  float m_maxAngle;

private:
  Blade &m_blade;
  Block &m_block;
  std::list<std::pair<math137::Vector3f, float>> m_postions;
  static constexpr float c_stepSize = .1f;
};
