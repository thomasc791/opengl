#ifndef SHADER_STORAGE_BUFFER_H
#define SHADER_STORAGE_BUFFER_H

#include <array>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class ShaderStorageBuffer {
public:
  GLuint ssbo;
  unsigned int m_ssbNum;
  int m_width, m_height;

  ShaderStorageBuffer(unsigned int ssbNum);
  ~ShaderStorageBuffer();

  void storeData(const void *data_ptr, size_t bufferSize) const;
  void updateData(const void *data_ptr, size_t bufferSize) const;
  void bindSSB();
  void unbindSSB();

private:
};

#endif
