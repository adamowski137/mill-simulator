#pragma once

#include "Entity.hpp"
#include <cstdint>
#include <memory>
class Ground : public Entity {
public:
  Ground();
  void render(const std::unique_ptr<Renderer> &renderer) override;

protected:
  void recalculateModel() override { m_update = false; }

private:
  static constexpr float c_gridSize = 10.f;
  static constexpr float c_gapSize = 1.f;
  static constexpr uint16_t c_gridCount = 24 * (c_gridSize / c_gapSize);

  std::vector<float> getGrid();
};
