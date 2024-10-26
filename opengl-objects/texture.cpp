#include "texture.h"

Texture::Texture(unsigned int texNum, unsigned int width, unsigned int height)
    : texNum(texNum), width(width), height(height) {
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0 + texNum);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glBindImageTexture(texNum, texture, 0, GL_FALSE, 0, GL_READ_WRITE,
                     GL_RGBA32F);
  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() { glDeleteTextures(1, &texture); }

void Texture::bindTexture() {
  glActiveTexture(GL_TEXTURE0 + texNum);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbindTexture() {
  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
