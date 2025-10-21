#include "ComputeShader.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

ComputeShader::ComputeShader(const std::string &path) {
  std::ifstream shaderFile;

  shaderFile.open(path);

  std::stringstream stream;

  stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  stream << shaderFile.rdbuf();

  shaderFile.close();
  std::string fileString = stream.str();
  const char *shaderCode = fileString.c_str();
  unsigned int compute;
  // compute shader
  compute = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(compute, 1, &shaderCode, NULL);
  glCompileShader(compute);
  checkCompileErrors(compute, "COMPUTE");

  // shader Program
  m_id = glCreateProgram();
  glAttachShader(m_id, compute);
  glLinkProgram(m_id);
  checkCompileErrors(m_id, "PROGRAM");
}

void ComputeShader::use() { glUseProgram(m_id); }

void ComputeShader::execute(uint32_t localX, uint32_t localY, uint32_t localZ) {
  glDispatchCompute(localX, localY, localZ);
}
void ComputeShader::await(GLbitfield barriers) { glMemoryBarrier(barriers); }
void ComputeShader::checkCompileErrors(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "Program") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      throw std::runtime_error(type + infoLog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      throw std::runtime_error(type + infoLog);
    }
  }
}
