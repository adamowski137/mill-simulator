#pragma once

#include "../Move.hpp"
#include "Entity.hpp"
#include <vector>
class Path : public Entity {
public:
  Path();
  void setMoves(const std::vector<Move> &moves);
  virtual void render(const std::unique_ptr<Renderer> &renderer) override;

private:
  uint32_t m_size;
  virtual void recalculateModel() override { m_update = false; }
};
