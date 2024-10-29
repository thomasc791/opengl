#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "texture.h"
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Framebuffer {
public:
  Framebuffer(Texture *texture);
  ~Framebuffer();

  void rescaleFramebuffer(unsigned int width, unsigned int height);
  void bindFramebuffer();
  void unbindFramebuffer();

private:
  GLuint m_fbo, m_rbo;
  Texture &m_texture;
};

#endif
