#pragma once

#include "Entity.hpp"
#include <cstdint>
#include <memory>

class Blade : public Entity {
public:
  Blade();
  void render(const std::unique_ptr<Renderer> &renderer) override;

protected:
  void recalculateModel() override;

private:
  static constexpr uint32_t c_segments = 32;
  static constexpr float c_height = 1.f;
  static constexpr float c_halfHeight = c_height * 0.5f;
  static constexpr float c_radius = 0.1f;
  std::vector<float> getGrid();
  std::vector<uint16_t> getIndices();
};
