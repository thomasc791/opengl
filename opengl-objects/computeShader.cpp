#include "computeShader.h"
#include <fstream>

ComputeShader::ComputeShader(std::string fileName) : fileName(fileName) {
  this->update();
}

ComputeShader::~ComputeShader() { glDeleteProgram(this->ID); }

void ComputeShader::use() { glUseProgram(ID); }

GLuint ComputeShader::getUniformLocation(const char *texName) {
  return glGetUniformLocation(ID, texName);
}

void ComputeShader::setFloat(const std::string &var, float f) const {
  glUniform1f(glGetUniformLocation(ID, var.c_str()), f);
}
void ComputeShader::setInt(const std::string &var, int i) const {
  glUniform1i(glGetUniformLocation(ID, var.c_str()), i);
}

void ComputeShader::setUIvec2(const std::string &var, uint x, uint y) const {
  glUniform2ui(glGetUniformLocation(ID, var.c_str()), x, y);
}

void ComputeShader::update() {
  std::ifstream csFile;
  csFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    std::string cp = std::filesystem::current_path();
    std::string fp = cp + "/shaders/" + fileName + ".cs.glsl";
    std::cout << "Reading Compute Shader: \n" << fp << std::endl;
    csFile.open(fp);
    std::stringstream buf;
    buf << csFile.rdbuf();
    computeShaderSource = buf.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "Error could not read file: " << e.what() << std::endl;
  }

  const char *csSource = computeShaderSource.c_str();
  unsigned int compute;
  compute = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(compute, 1, &csSource, NULL);
  glCompileShader(compute);
  this->checkCompileErrors(compute, "COMPUTE");

  ID = glCreateProgram();
  glAttachShader(ID, compute);
  glLinkProgram(ID);
  this->checkCompileErrors(ID, "PROGRAM");
  glDeleteShader(compute);
}

void ComputeShader::checkCompileErrors(const unsigned int shader,
                                       const std::string shaderType) const {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
};
