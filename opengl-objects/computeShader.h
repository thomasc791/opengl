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
  std::string project, fileName;
  std::string computeShaderSource, cs;

  ComputeShader(std::string project, std::string fileName);
  ~ComputeShader();

  void use();
  GLuint getUniformLocation(const char *texName);
  void setInt(const std::string &var, int i) const;
  void setUint(const std::string &var, uint i) const;
  void setFloat(const std::string &var, float f) const;
  void setUIvec2(const std::string &var, uint x, uint y) const;
  void update();

private:
  void checkCompileErrors(const unsigned int shader,
                          const std::string shaderType) const;
};

#endif
