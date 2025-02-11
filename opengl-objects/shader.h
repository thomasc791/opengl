#ifndef SHADER_H
#define SHADER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Shader {
public:
  unsigned int ID;
  std::string project;

  Shader(std::string project, std::string vsFileName, std::string fsFileName);
  ~Shader();

  GLuint getUniformLocation(const char *uniform);
  void setInt(const std::string &name, float value) const;
  void use() const;

private:
  void checkCompileErrors(unsigned int shader, std::string shaderType);
};

#endif
