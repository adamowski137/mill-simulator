#include "Simulator.hpp"
#include "Vector.hpp"
#include "models/Blade.hpp"
#include <cmath>
#include <iostream>
#include <list>

Simulator::Simulator(Blade &blade) : m_blade(blade) {}

void Simulator::setMoves(const std::vector<Move> &moves) {
  math137::Vector3f f = {moves[0].x.value(), moves[0].y.value(),
                         moves[0].z.value()};
  f = f / c_scale;
  for (int i = 1; i < moves.size() - 1; i++) {
    math137::Vector3f n = {moves[i].x.value_or(f.x()),
                           moves[i].y.value_or(f.y()),
                           moves[i].z.value_or(f.z())};
    n = n / c_scale;
    math137::Vector3f dir = n - f;
    float length =
        sqrtf(dir.x() * dir.x() + dir.y() * dir.y() + dir.z() * dir.z());
    dir.normalize();
    int steps = static_cast<int>(length / c_stepSize);
    for (int i = 0; i < steps; i++) {
      m_postions.push_back(f + (dir * (i * c_stepSize)));
    }
    f = n;
  }
}
void Simulator::simulate(float speed) {
  for (int i = 0; i < 1000; i++) {
    if (m_postions.empty())
      return;
    m_blade.setTranslation(m_postions.front());
    m_postions.pop_front();
  }
}
