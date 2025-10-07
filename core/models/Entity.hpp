#pragma once

#include "../Renderer.hpp"
#include "../Shader.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include <cstdint>
#include <memory>
class Entity {
public:
  Entity(ShaderType type, const math137::Vector3f &pos);
  ~Entity();

  virtual void render(const std::unique_ptr<Renderer> &renderer) = 0;

  void setTranslation(const math137::Vector3f &pos);
  math137::Vector3f getTranslation() const;
  math137::Matrix4f getModel();

protected:
  virtual void recalculateModel();
  bool m_update;
  ShaderType m_type;

  uint32_t m_vao;
  uint32_t m_vbo;
  uint32_t m_ebo;

  math137::Vector3f m_translation;
  math137::Matrix4f m_model;
};
