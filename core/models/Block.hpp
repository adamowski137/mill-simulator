#pragma once

#include "../ComputeShader.hpp"
#include "Blade.hpp"
#include "Entity.hpp"
#include "Vertex.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#pragma one

class Block : public Entity {
public:
  Block();

  virtual void render(const std::unique_ptr<Renderer> &renderer) override;
  void changeResolution(uint32_t resX, uint32_t resY);
  uint32_t mill(const Blade &blade);
  void reset();
  void setBuffer();

  uint32_t m_resX, m_resZ;
  float m_sizeX, m_sizeY, m_sizeZ;
  float m_baseHeight;
  GLuint m_atomicBuffer;

private:
  std::vector<uint32_t> getIndices();
  std::vector<VertexNormal> getBorderVertices();
  virtual void recalculateModel() override;
  std::vector<float> m_heightMap;
  std::vector<uint32_t> m_indices;
  std::vector<VertexNormal> m_vertices;
  GLuint m_heightTex;
  GLuint m_blockTex;
  ComputeShader m_gridShader;
  ComputeShader m_millShader;
};
