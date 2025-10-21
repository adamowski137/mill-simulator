#pragma once

#include "Simulator.hpp"
#include "models/Blade.hpp"
#include "models/Block.hpp"
#include "models/Ground.hpp"
#include "models/Path.hpp"
#include <memory>
#include <vector>
class Scene {
public:
  Scene();

  void render(const std::unique_ptr<Renderer> &renderer);
  void setSimulatorMoves(const std::vector<Move> &moves);
  void startSimulator();
  void finishSimulator();
  void setSimulatorSpeed(int speed);
  void setMaxAngle(float angle) { m_simulator.m_maxAngle = angle; }
  void update();
  void setBlockData(int resX, int resZ, float sizeX, float sizeY, float sizeZ,
                    float baseHeight);

  void setBladeData(float raius, float height, BladeType type);
  inline void setBladeRadius(float radius) { m_blade.m_radius = radius; }
  inline void setBladeType(BladeType b) { m_blade.m_bladeType = b; }

private:
  Ground m_ground;
  Blade m_blade;
  Block m_block;
  Path m_path;
  Simulator m_simulator;
};
