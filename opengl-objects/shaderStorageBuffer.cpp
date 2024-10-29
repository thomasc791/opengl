#include "shaderStorageBuffer.h"

ShaderStorageBuffer::ShaderStorageBuffer(unsigned int ssbNum)
    : m_ssbNum(ssbNum) {
  glGenBuffers(1, &ssbo);
}

ShaderStorageBuffer::~ShaderStorageBuffer() { glDeleteBuffers(1, &ssbo); }

void ShaderStorageBuffer::storeData(const void *data_ptr, size_t bufferSize) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, data_ptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_ssbNum, ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::bindSSB() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
}

void ShaderStorageBuffer::unbindSSB() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
