#ifndef COMPUTESIMPLE_H
#define COMPUTESIMPLE_H

#include "imgui-src/imgui.h"
#include "imgui-src/imgui_impl_glfw.h"
#include "imgui-src/imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
std::string readShader(std::string fileName);
void checkCompileErrors(unsigned int shader, std::string shaderType);
void renderQuad(GLuint &VAO);
void createBox();
void createTexture(GLuint &texture);
void rescaleFramebuffer(float width, float height, GLuint &RBO,
                        GLuint &texture);
#endif
