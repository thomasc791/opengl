#ifndef TEXTURE_H
#define TEXTURE_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Texture {
public:
  GLuint texture;
  unsigned int texNum, width, height;

  Texture(unsigned int texNum, unsigned int width, unsigned int height);
  ~Texture();

  void bindTexture();
  void unbindTexture();
};

#endif
