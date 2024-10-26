#include "shader.h"
#include <exception>
#include <fstream>

Shader::Shader(std::string vsFileName, std::string fsFileName) {
  std::string vertexShaderSource;
  std::string fragmentShaderSource;

  std::string vs, fs;

  std::ifstream vsFile, fsFile;
  vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    std::string cp = std::filesystem::current_path();
    vs = cp + "/shaders/" + vsFileName;
    fs = cp + "/shaders/" + fsFileName;

    std::cout << "Reading files: \n" << vs << "\n" << fs << std::endl;
    vsFile.open(vs);
    std::stringstream vsBuf;
    vsBuf << vsFile.rdbuf();
    vertexShaderSource = vsBuf.str();

    std::ifstream fsFile(fs);
    std::stringstream fsBuf;
    fsBuf << fsFile.rdbuf();
    fragmentShaderSource = fsBuf.str();
    vsFile.close();
    fsFile.close();
  } catch (std::exception &e) {
    std::cout << "Error could not read file: " << e.what() << std::endl;
    std::cout << "Check if files exists: \n" << vs << "\n" << fs << std::endl;
  }

  const char *vsSource = vertexShaderSource.c_str();
  const char *fsSource = fragmentShaderSource.c_str();
  unsigned int vsShader, fsShader;

  vsShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsShader, 1, &vsSource, NULL);
  glCompileShader(vsShader);
  checkCompileErrors(vsShader, "VERTEX");

  fsShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsShader, 1, &fsSource, NULL);
  glCompileShader(fsShader);
  checkCompileErrors(fsShader, "FRAGMENT");

  ID = glCreateProgram();
  glAttachShader(ID, vsShader);
  glAttachShader(ID, fsShader);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");
  glDeleteShader(vsShader);
  glDeleteShader(fsShader);
}

Shader::~Shader() { glDeleteProgram(ID); }

GLuint Shader::getUniformLocation(const char *uniform) {
  return glGetUniformLocation(ID, uniform);
}
void Shader::setInt(const std::string &name, float value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::use() const { glUseProgram(ID); }

void Shader::checkCompileErrors(unsigned int shader, std::string shaderType) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
};
