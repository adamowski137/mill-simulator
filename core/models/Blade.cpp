#include "Blade.hpp"
#include "MatrixUtils.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <cmath>
#include <cstdint>
#include <vector>

Blade::Blade() : Entity(ShaderType::OBJECT, {0.f, 1.f, 0.f}) {
  glBindVertexArray(m_vao);
  auto vertices = getGrid();
  auto indices = getIndices();
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t),
               indices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(0);
}

void Blade::render(const std::unique_ptr<Renderer> &renderer) {
  glBindVertexArray(m_vao);
  renderer->setShader(m_type);
  renderer->setModel(Entity::getModel());
  renderer->setColor({1.f, 1.f, 1.f, 1.f});
  glDrawElements(GL_TRIANGLES, c_segments * 12, GL_UNSIGNED_SHORT, (void *)0);
  glBindVertexArray(0);
}

std::vector<float> Blade::getGrid() {
  std::vector<float> vertices;
  vertices.reserve((c_segments + 1) * 2 * 3 + 6);

  for (uint32_t i = 0; i <= c_segments; ++i) {
    float theta = (2.f * M_PIf * i) / c_segments;
    float x = std::cos(theta) * c_radius;
    float z = std::sin(theta) * c_radius;
    vertices.push_back(x);
    vertices.push_back(-c_halfHeight);
    vertices.push_back(z);

    vertices.push_back(x);
    vertices.push_back(c_halfHeight);
    vertices.push_back(z);
  }

  vertices.push_back(0.f);
  vertices.push_back(-c_halfHeight);
  vertices.push_back(0.f);

  vertices.push_back(0.f);
  vertices.push_back(c_halfHeight);
  vertices.push_back(0.f);

  return vertices;
}

std::vector<uint16_t> Blade::getIndices() {
  std::vector<uint16_t> indices;
  indices.reserve(c_segments * 12);

  for (uint32_t i = 0; i < c_segments; ++i) {
    uint16_t bottom0 = i * 2;
    uint16_t top0 = i * 2 + 1;
    uint16_t bottom1 = (i + 1) * 2;
    uint16_t top1 = (i + 1) * 2 + 1;

    indices.push_back(bottom0);
    indices.push_back(top0);
    indices.push_back(bottom1);

    indices.push_back(top0);
    indices.push_back(top1);
    indices.push_back(bottom1);
  }

  uint16_t bottomCenterIndex = static_cast<uint16_t>((c_segments + 1) * 2);
  uint16_t topCenterIndex = bottomCenterIndex + 1;

  for (uint32_t i = 0; i < c_segments; ++i) {
    uint16_t v0 = i * 2;
    uint16_t v1 = ((i + 1) % (c_segments + 1)) * 2;
    indices.push_back(bottomCenterIndex);
    indices.push_back(v1);
    indices.push_back(v0);
  }

  // Top cap indices
  for (uint32_t i = 0; i < c_segments; ++i) {
    uint16_t v0 = i * 2 + 1;
    uint16_t v1 = ((i + 1) % (c_segments + 1)) * 2 + 1;
    indices.push_back(topCenterIndex);
    indices.push_back(v0);
    indices.push_back(v1);
  }
  return indices;
}
void Blade::recalculateModel() {
  m_model = math137::MatrixUtils::Translate(
      m_translation.x(), m_translation.y(), m_translation.z());
  m_update = false;
}
