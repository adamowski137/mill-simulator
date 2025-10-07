#pragma once

#include "Simulator.hpp"
#include "models/Blade.hpp"
#include "models/Ground.hpp"
#include <memory>
#include <vector>
class Scene {
public:
  Scene();

  void render(const std::unique_ptr<Renderer> &renderer);
  void setSimulatorMoves(const std::vector<Move> &moves);
  void startSimulator();
  void update();

private:
  bool m_simulate;
  Ground m_ground;
  Blade m_blade;
  Simulator m_simulator;
};
