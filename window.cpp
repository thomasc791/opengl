#include "window.h"

#include "imgui-src/imgui.h"
#include "imgui-src/imgui_impl_glfw.h"

#include "opengl-objects/computeShader.h"
#include "opengl-objects/framebuffer.h"
#include "opengl-objects/shader.h"
#include "opengl-objects/shaderStorageBuffer.h"
#include "opengl-objects/texture.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec4.hpp>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int TEXTURE_WIDTH = 1920, TEXTURE_HEIGHT = 1080;

GLuint VAO, VBO, FBO, RBO;

int WINDOW_WIDTH, WINDOW_HEIGHT;

int main() {
  GLFWwindow *window;
  if (glfwSetup(window) == -1)
    return -1;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  ImGuiWindowFlags wFlags;
  // wFlags |= ImGuiWindowFlags_NoTitleBar;
  // wFlags |= ImGuiWindowFlags_NoResize;
  // wFlags |= ImGuiWindowFlags_NoScrollbar;
  // wFlags |= ImGuiWindowFlags_NoScrollWithMouse;

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.5f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Shader creation
  Shader shader("vertexShader.vs.glsl", "fragmentShader.fs.glsl");
  ComputeShader computeShader("computeShader.cs.glsl");

  // Texture creation
  Texture texFB(0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
  Texture texCS(1, TEXTURE_WIDTH, TEXTURE_HEIGHT);

  Framebuffer fbo(&texFB);

  std::vector<std::vector<glm::vec4>> data(
      TEXTURE_WIDTH, std::vector<glm::vec4>(TEXTURE_HEIGHT));

  for (size_t i = 0; i < TEXTURE_WIDTH; i++) {
    for (size_t j = 0; j < TEXTURE_HEIGHT; j++) {
      data[i][j] = {
          ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)),
          ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX))};
    }
  }

  ShaderStorageBuffer ssbo(0);

  ssbo.storeData((const void *)data.data(),
                 sizeof(glm::vec4) * TEXTURE_WIDTH * TEXTURE_HEIGHT);

  GLuint vbo, vao;
  std::array<int, 4> screenPosArray{};
  int windowPadding = 1;
  glfwGetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
  std::array<int, 2> buttonFrameSize = {200, WINDOW_WIDTH};

  GLuint imgOutputLocation = computeShader.getUniformLocation("imgOutput");
  GLuint texLocation = shader.getUniformLocation("tex");

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  const char *fps = "FPS";
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    // processInput(window);

    auto startTime = glfwGetTime();
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    fbo.bindFramebuffer();
    ImGui::NewFrame();
    {
      ImGui::SetNextWindowSize(ImVec2(buttonFrameSize[0], WINDOW_HEIGHT));
      // ImGui::SetCursorPos(ImVec2(0, 0));
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      // Create Buttons window
      ImGui::Begin("Buttons");
      ImGui::Button(fps);
      ImGui::End();
    }

    {
      screenPosArray[2] = screenPosArray[0] + buttonFrameSize[0];
      ImGui::SetNextWindowPos(ImVec2(screenPosArray[2], screenPosArray[3]));
      ImGui::SetNextWindowSize(
          ImVec2(WINDOW_WIDTH - screenPosArray[2], WINDOW_HEIGHT));
      ImGui::Begin("Scene");
      const float windowWidth = ImGui::GetContentRegionAvail().x;
      const float windowHeight = ImGui::GetContentRegionAvail().y;

      fbo.rescaleFramebuffer(windowWidth, windowHeight);
      glViewport(0, 0, windowWidth, windowHeight);

      ImVec2 pos = ImGui::GetCursorScreenPos();

      ImGui::Image((ImTextureID)(intptr_t)texCS.texture,
                   ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }

    ImGui::Render();
    fbo.unbindFramebuffer();

    texCS.bindTexture();
    ssbo.bindSSB();

    computeShader.use();
    computeShader.setFloat("t", startTime);
    glUniform1i(imgOutputLocation, texCS.texNum);
    glDispatchCompute((unsigned int)TEXTURE_WIDTH / 128,
                      (unsigned int)TEXTURE_HEIGHT / 8, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ssbo.unbindSSB();
    texCS.unbindTexture();
    // glUniform1i(texLocation, texCS.texNum);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backupCurrentContext = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backupCurrentContext);
    }

    glfwSwapBuffers(window);

    fps = std::to_string((glfwGetTime() - startTime)).c_str();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void createBox(GLuint &vbo, GLuint &vao) {
  float quadVertices[] = {
      // positions        // texture Coords
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  // setup plane vao
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
}

void renderQuad(GLuint &vao) {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina
  // displays.
  WINDOW_WIDTH = width;
  WINDOW_HEIGHT = height;
  glViewport(0, 0, width, height);
}

int glfwSetup(GLFWwindow *&window) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test Window", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(0);
  return 0;
}
