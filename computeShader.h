#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <sstream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <iostream>

class ComputeShader {
public:
  unsigned int ID;

  ComputeShader(std::string fileName);
  ~ComputeShader();

  void use();
  GLuint getUniformLocation(const char *texName);

private:
  void checkCompileErrors(unsigned int shader, std::string shaderType);
};

#endif
