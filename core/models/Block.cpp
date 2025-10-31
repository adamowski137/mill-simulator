#define STB_IMAGE_IMPLEMENTATION

#include "Block.hpp"
#include "../stb_image.h"
#include "Blade.hpp"
#include "Entity.hpp"
#include "MatrixUtils.hpp"
#include "Vector.hpp"
#include "Vertex.hpp"
#include <GL/gl.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

Block::Block()
    : Entity(ShaderType::BLOCK, {0.f, 0.f, 0.f}), m_resX(600), m_resZ(600),
      m_sizeX(150.f), m_sizeY(50.f), m_sizeZ(150.f), m_baseHeight(15.f),
      m_heightMap(m_resX * m_resZ, m_sizeY), m_gridShader("shaders/grid.comp"),
      m_millShader("shaders/mill.comp") {
  auto vert = getBorderVertices();
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               (m_resX * m_resZ + 2 * (m_resX + m_resZ)) *
                   (sizeof(VertexNormal)),
               vert.data(), GL_DYNAMIC_DRAW);
  auto indices = getIndices();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
               indices.data(), GL_STATIC_DRAW);
  glBindVertexArray(m_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal),
                        (void *)offsetof(VertexNormal, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal),
                        (void *)offsetof(VertexNormal, norm));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal),
                        (void *)offsetof(VertexNormal, tex));
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  glGenTextures(1, &m_blockTex);
  glBindTexture(GL_TEXTURE_2D, m_blockTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  unsigned char *data =
      stbi_load("shaders/wood2.jpeg", &width, &height, &channels, 0);
  GLenum format;
  switch (channels) {
  case 1:
    format = GL_LUMINANCE;
    break;
  case 2:
    format = GL_LUMINANCE_ALPHA;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  glGenTextures(1, &m_heightTex);
  glBindTexture(GL_TEXTURE_2D, m_heightTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_resX, m_resZ, 0, GL_RED, GL_FLOAT,
               m_heightMap.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  setTranslation({-m_sizeX / 2.f, 0, -m_sizeZ / 2.f});

  glGenBuffers(1, &m_atomicBuffer);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);
  setBuffer();
  recalculateModel();
}
std::vector<VertexNormal> Block::getBorderVertices() {
  std::vector<VertexNormal> res;
  res.resize(2 * (m_resX + m_resZ) + m_resX * m_resZ);

  int i = 0;
  for (int x = 0; x < m_resX; x++) {
    VertexNormal v{};
    v.pos[0] = m_sizeX * 0.1f * x / (m_resX - 1);
    v.pos[1] = 0.f;
    v.pos[2] = 0.f;
    v.pos[3] = 1.f;
    v.norm[0] = 0.f;
    v.norm[1] = 0.f;
    v.norm[2] = -1.f;
    v.norm[3] = 0.f;
    v.tex[0] = (float)x / (m_resX - 1);
    v.tex[1] = 1.f;
    res[i++] = (v);
  }

  for (int z = 0; z < m_resZ; z++) {
    VertexNormal v{};
    v.pos[0] = m_sizeX * 0.1f;
    v.pos[1] = 0.f;
    v.pos[2] = m_sizeZ * 0.1f * z / (m_resZ - 1);
    v.pos[3] = 1.f;
    v.norm[0] = 1.f;
    v.norm[1] = 0.f;
    v.norm[2] = 0.f;
    v.norm[3] = 0.f;
    v.tex[0] = 0.f;
    v.tex[1] = (float)z / (m_resZ - 1);
    res[i++] = (v);
  }

  for (int x = 0; x < m_resX; x++) {
    VertexNormal v{};
    v.pos[0] = m_sizeX * 0.1f * x / (m_resX - 1);
    v.pos[1] = 0.f;
    v.pos[2] = m_sizeZ * 0.1f;
    v.pos[3] = 1.f;
    v.norm[0] = 0.f;
    v.norm[1] = 0.f;
    v.norm[2] = 1.f;
    v.norm[3] = 0.f;
    v.tex[0] = (float)x / (m_resX - 1);
    v.tex[1] = 0.f;
    res[i++] = (v);
  }

  for (int z = 0; z < m_resZ; z++) {
    VertexNormal v{};
    v.pos[0] = 0.f;
    v.pos[1] = 0.f;
    v.pos[2] = 0.1f * m_sizeZ * z / (m_resZ - 1);
    v.pos[3] = 1.f;
    v.norm[0] = -1.f;
    v.norm[1] = 0.f;
    v.norm[2] = 0.f;
    v.norm[3] = 0.f;
    v.tex[0] = 1.f;
    v.tex[1] = (float)z / (m_resZ - 1);
    res[i++] = (v);
  }

  return res;
}

std::vector<uint32_t> Block::getIndices() {
  std::vector<uint32_t> indices;

  // GPU grid vertices start after border vertices
  uint32_t gridOffset = 2 * (m_resX + m_resZ);
  uint32_t gridWidth = m_resX;
  uint32_t gridHeight = m_resZ;

  // Reserve enough for top + sides
  indices.reserve((m_resX - 1) * (m_resZ - 1) * 6 + 6 * 2 * (m_resX + m_resZ));

  // === 1. Top surface triangles (GPU-generated) ===
  for (uint32_t x = 0; x < gridWidth - 1; ++x) {
    for (uint32_t z = 0; z < gridHeight - 1; ++z) {
      uint32_t i0 = gridOffset + x * gridHeight + z;
      uint32_t i1 = i0 + 1;
      uint32_t i2 = i0 + gridHeight;
      uint32_t i3 = i2 + 1;

      indices.push_back(i0);
      indices.push_back(i2);
      indices.push_back(i1);

      indices.push_back(i1);
      indices.push_back(i2);
      indices.push_back(i3);
    }
  }

  // === 2. Side walls (CPU-generated) ===
  // The order of border vertices in getBorderVertices():
  //  [0 .. m_resX-1]            -> bottom edge  (z=0)
  //  [m_resX .. m_resX+m_resZ-2] -> right edge   (x=max)
  //  [m_resX+m_resZ-1 .. m_resX+m_resZ-1+(m_resX-1)] -> top edge reversed
  //  (z=max) [rest] -> left edge reversed (x=0)

  uint32_t borderBottom = 0;
  uint32_t borderRight = m_resX;
  uint32_t borderTop = borderRight + m_resZ;
  uint32_t borderLeft = borderTop + m_resX;

  // --- Bottom edge (z = 0) ---
  for (uint32_t x = 0; x < m_resX - 1; ++x) {
    uint32_t v0 = borderBottom + x;
    uint32_t v1 = borderBottom + x + 1;
    uint32_t t0 = gridOffset + x * m_resZ;
    uint32_t t1 = gridOffset + (x + 1) * m_resZ;

    indices.push_back(v0);
    indices.push_back(t0);
    indices.push_back(t1);

    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(t1);
  }

  // --- Right edge (x = max) ---
  for (uint32_t z = 0; z < m_resZ - 1; ++z) {
    uint32_t v0 = borderRight + z;
    uint32_t v1 = borderRight + z + 1;
    uint32_t t0 = gridOffset + (m_resX - 1) * m_resZ + z;
    uint32_t t1 = t0 + 1;

    indices.push_back(v0);
    indices.push_back(t1);
    indices.push_back(t0);

    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(t1);
  }

  // --- Top edge (z = max) ---
  for (uint32_t x = 0; x < m_resX - 1; ++x) {
    uint32_t v0 = borderTop + x;
    uint32_t v1 = borderTop + x + 1;
    uint32_t t0 = gridOffset + x * m_resZ + (m_resZ - 1);
    uint32_t t1 = gridOffset + (x + 1) * m_resZ + (m_resZ - 1);

    indices.push_back(v0);
    indices.push_back(t1);
    indices.push_back(t0);

    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(t1);
  }

  // --- Left edge (x = 0) ---
  for (uint32_t z = 0; z < m_resZ - 1; ++z) {
    uint32_t v0 = borderLeft + z;
    uint32_t v1 = borderLeft + z + 1;
    uint32_t t0 = gridOffset + z;
    uint32_t t1 = t0 + 1;

    indices.push_back(v0);
    indices.push_back(t1);
    indices.push_back(t0);

    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(t1);
  }

  return indices;
}

void Block::setBuffer() {
  m_gridShader.use();
  glBindTexture(GL_TEXTURE_2D, m_heightTex);
  glBindImageTexture(0, m_heightTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo);
  uint32_t groupSize = 32;
  uint32_t groupsX = (m_resX + groupSize - 1) / groupSize;
  uint32_t groupsY = (m_resZ + groupSize - 1) / groupSize;
  m_gridShader.setUInt("gridHeight", m_resZ);
  m_gridShader.setUInt("gridWidth", m_resX);
  m_gridShader.setVec2("size", m_sizeX, m_sizeZ);

  m_gridShader.execute(groupsX, groupsY);
  m_gridShader.await(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Block::recalculateModel() {
  m_update = false;
  m_model = math137::MatrixUtils::Translate(m_translation.x() / 10.f,
                                            m_translation.y() / 10.f,
                                            m_translation.z() / 10.f);
}

void Block::changeResolution(uint32_t resX, uint32_t resZ) {
  m_resX = resX;
  m_resZ = resZ;
  reset();
}

void Block::reset() {
  m_heightMap.resize(m_resX * m_resZ, m_sizeY);
  glBindVertexArray(m_vao);
  auto vert = getBorderVertices();
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               (m_resX * m_resZ + 2 * (m_resX + m_resZ)) *
                   (sizeof(VertexNormal)),
               vert.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  auto indices = getIndices();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
               indices.data(), GL_STATIC_DRAW);
  glBindTexture(GL_TEXTURE_2D, m_heightTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_resX, m_resZ, 0, GL_RED, GL_FLOAT,
               m_heightMap.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  setBuffer();
}
uint32_t Block::mill(const Blade &blade) {
  // Transform blade position into block-local space
  math137::Vector3f millPos = blade.getTranslation() - getTranslation();
  math137::Vector3f blockSize = {m_sizeX, m_sizeY, m_sizeZ};
  math137::Vector<int, 2> res = {(int)m_resX, (int)m_resZ};
  bool ballBlade = blade.m_bladeType == BladeType::BALL;
  float radius = blade.m_radius;
  float bladeHeight = blade.m_height;
  m_millShader.use();
  m_millShader.setVec3("bladePos", millPos);
  m_millShader.setVec3("blockSize", blockSize);
  m_millShader.setVec2i("resolution", res);
  m_millShader.setBool("ballBlade", ballBlade);
  m_millShader.setFloat("bladeRadius", radius);
  m_millShader.setFloat("bladeHeight", bladeHeight);
  m_millShader.setFloat("baseHeight", m_baseHeight);
  GLuint zero = 0;
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, m_atomicBuffer);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_heightTex);
  glBindImageTexture(0, m_heightTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
  
  uint32_t groupSize = 32;
  
  float squareSideWorld = 2.0f * radius;
  
  float leftBottomX = millPos.x() - radius;
  float leftBottomZ = millPos.z() - radius;
  
  float leftBottomTexX = leftBottomX / m_sizeX;
  float leftBottomTexZ = leftBottomZ / m_sizeZ;
  
  int leftBottomPixelX = (int)(leftBottomTexX * m_resX);
  int leftBottomPixelZ = (int)(leftBottomTexZ * m_resZ);

  m_millShader.setVec2i("leftBottomPixel",
                         {leftBottomPixelX, leftBottomPixelZ});
  
  int pixelsX = (int)((squareSideWorld / m_sizeX) * m_resX);
  int pixelsY = (int)((squareSideWorld / m_sizeZ) * m_resZ);

  m_millShader.setVec2i("pixels",
                         {pixelsX, pixelsY});

  uint32_t groupsX = (pixelsX + groupSize - 1) / groupSize;
  uint32_t groupsY = (pixelsY + groupSize - 1) / groupSize;
  
  groupsX = std::max(1u, groupsX);
  groupsY = std::max(1u, groupsY);
  
  m_millShader.execute(groupsX, groupsY);
  m_millShader.await(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                     GL_ATOMIC_COUNTER_BUFFER);
  GLuint bits = 0;
  glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &bits);
  glBindTexture(GL_TEXTURE_2D, 0);
  return bits;
}

void Block::render(const std::unique_ptr<Renderer> &renderer) {
  renderer->setShader(m_type);
  renderer->setModel(Entity::getModel());
  renderer->setBlockData(m_sizeX, m_sizeY, m_sizeZ);
  glBindVertexArray(m_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_blockTex);
  // uint32_t count = (m_resX - 1) * (m_resZ - 1) * 6 + 6 * (m_resX + m_resZ);
  uint32_t count = (m_resX - 1) * (m_resZ - 1) * 6 +
                   6 * (2 * (m_resX - 1) + 2 * (m_resZ - 1));
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void *)0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
}
