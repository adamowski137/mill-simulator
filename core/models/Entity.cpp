#include "Entity.hpp"
#include "MatrixUtils.hpp"
#include "Vector.hpp"

Entity::Entity(ShaderType type, const math137::Vector3f &translation)
    : m_translation(translation), m_type(type), m_update(false),
      m_model(math137::MatrixUtils::Identity()) {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_ebo);
  glGenBuffers(1, &m_vbo);
}

Entity::~Entity() {
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_ebo);
  glDeleteBuffers(1, &m_vbo);
}

void Entity::setTranslation(const math137::Vector3f &pos) {
  m_translation = pos;
  m_update = true;
}

math137::Vector3f Entity::getTranslation() const { return m_translation; }

math137::Matrix4f Entity::getModel() {
  if (m_update)
    recalculateModel();

  return m_model;
}

void Entity::recalculateModel() {
  m_model = math137::MatrixUtils::Translate(
      m_translation.x(), m_translation.y(), m_translation.z());
  m_update = false;
}
