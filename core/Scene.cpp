#include "Scene.hpp"
#include <memory>
#include <vector>
Scene::Scene() : m_simulator(m_blade), m_simulate(false) {}

void Scene::render(const std::unique_ptr<Renderer> &renderer) {
  m_ground.render(renderer);
  m_blade.render(renderer);
}

void Scene::setSimulatorMoves(const std::vector<Move> &moves) {
  m_simulator.setMoves(std::move(moves));
}

void Scene::startSimulator() { m_simulate = true; }

void Scene::update() {
  if (m_simulate) {
    m_simulator.simulate(0.1f);
  }
}
