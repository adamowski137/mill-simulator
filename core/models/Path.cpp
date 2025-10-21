#include "Path.hpp"
#include "Entity.hpp"
#include "Vector.hpp"
#include <vector>

Path::Path() : Entity(ShaderType::OBJECT, {}), m_size(0) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
  glBindVertexArray(0);
}

void Path::setMoves(const std::vector<Move> &moves) {
  m_size = moves.size();
  std::vector<float> res;
  res.reserve(moves.size() * 3);
  float x = moves[0].x.value();
  float y = moves[0].y.value();
  float z = moves[0].z.value();

  res.push_back(x * 0.1f);
  res.push_back(y * 0.1f);
  res.push_back(z * -0.1f);

  for (int i = 1; i < moves.size(); i++) {
    x = moves[i].x.value_or(x);
    y = moves[i].y.value_or(y);
    z = moves[i].z.value_or(z);

    res.push_back(x * 0.1f);
    res.push_back(y * 0.1f);
    res.push_back(z * -0.1f);
  }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, res.size() * sizeof(float), res.data(),
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Path::render(const std::unique_ptr<Renderer> &renderer) {
  renderer->setShader(m_type);
  renderer->setModel(Entity::getModel());
  renderer->setColor({0.7f, 0.f, 1.f, 1.f});
  glBindVertexArray(m_vao);
  glDrawArrays(GL_LINE_STRIP, 0, m_size);
  glBindVertexArray(0);
}
