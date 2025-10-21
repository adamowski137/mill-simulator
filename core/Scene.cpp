#include "Scene.hpp"
#include "models/Blade.hpp"
#include <memory>
#include <vector>
Scene::Scene() : m_simulator(m_blade, m_block) {}

void Scene::render(const std::unique_ptr<Renderer> &renderer) {
  m_ground.render(renderer);
  m_blade.render(renderer);
  m_block.render(renderer);
  m_path.render(renderer);
}

void Scene::setSimulatorMoves(const std::vector<Move> &moves) {
  m_path.setMoves(moves);
  m_simulator.setMoves(std::move(moves));
}

void Scene::startSimulator() { m_simulator.m_simulate = true; }
void Scene::finishSimulator() { m_simulator.finish(); }
void Scene::setSimulatorSpeed(int speed) { m_simulator.m_speed = 100 * speed; }

void Scene::update() { m_simulator.simulate(); }
void Scene::setBlockData(int resX, int resZ, float sizeX, float sizeY,
                         float sizeZ, float baseHeight) {
  m_block.m_sizeX = sizeX * 10.f;
  m_block.m_sizeY = sizeY * 10.f;
  m_block.m_sizeZ = sizeZ * 10.f;
  m_block.m_resX = resX;
  m_block.m_resZ = resZ;
  m_block.m_baseHeight = baseHeight;
  m_block.reset();
}
void Scene::setBladeData(float radius, float height, BladeType type) {
  m_blade.m_height = height;
  m_blade.m_radius = radius;
  m_blade.m_bladeType = type;
}
