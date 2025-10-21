#include "Simulator.hpp"
#include "Vector.hpp"
#include "models/Blade.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <list>

Simulator::Simulator(Blade &blade, Block &block)
    : m_blade(blade), m_block(block), m_simulate(false), m_speed(1000),
      m_maxAngle(M_PI_4f) {}

void Simulator::setMoves(const std::vector<Move> &moves) {
  m_postions.clear();
  math137::Vector3f f = {moves[0].x.value(), moves[0].y.value(),
                         -moves[0].z.value()};
  for (int i = 1; i < moves.size(); i++) {
    math137::Vector3f n = {moves[i].x.value_or(f.x()),
                           moves[i].y.value_or(f.y()),
                           -moves[i].z.value_or(f.z())};
    math137::Vector3f dir = n - f;
    float angle =
        std::atan2(dir.y(), std::sqrt(dir.x() * dir.x() + dir.z() * dir.z()));
    float length =
        sqrtf(dir.x() * dir.x() + dir.y() * dir.y() + dir.z() * dir.z());
    dir.normalize();
    int steps = static_cast<int>(length / c_stepSize);
    for (int i = 0; i < steps; i++) {
      m_postions.push_back(std::make_pair(f + (dir * (i * c_stepSize)), angle));
    }
    m_postions.push_back(std::make_pair(n, 0.0f));
    f = n;
  }
}
void Simulator::simulate() {
  if (!m_simulate)
    return;
  for (int i = 0; i < m_speed; i++) {
    if (m_postions.empty()) {
      m_simulate = false;
      break;
    }
    auto [pos, angle] = m_postions.front();
    m_blade.setTranslation(pos);
    m_postions.pop_front();
    uint32_t res = m_block.mill(m_blade);
    if ((res & 4) > 0) {
      std::cout << "Mill through base!" << std::endl;
      m_simulate = false;
      break;
    }
    if ((res & 2) > 0) {
      std::cout << "Mill with non sharp part" << std::endl;
      m_simulate = false;
      break;
    }
    if ((res & 1) > 0 && angle < -m_maxAngle) {
      std::cout << "Milling angle to steep" << std::endl;
      m_simulate = false;
      break;
    }
  }
  m_block.setBuffer();
}
void Simulator::finish() {
  while (!m_postions.empty()) {
    auto [pos, angle] = m_postions.front();
    m_blade.setTranslation(pos);
    m_postions.pop_front();
    uint32_t res = m_block.mill(m_blade);
    if ((res & 4) > 0) {
      std::cout << "Mill through base!" << std::endl;
      m_simulate = false;
      break;
    }
    if ((res & 2) > 0) {
      std::cout << "Mill with non sharp part" << std::endl;
      m_simulate = false;
      break;
    }
    if ((res & 1) > 0 && angle < -m_maxAngle) {
      std::cout << "Milling angle to steep" << std::endl;
      m_simulate = false;
      break;
    }
  }
  m_simulate = false;
  m_block.setBuffer();
}
