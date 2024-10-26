#include "window.h"

#include "imgui-src/imgui.h"
#include "imgui-src/imgui_impl_glfw.h"

#include "opengl-objects/computeShader.h"
#include "opengl-objects/framebuffer.h"
#include "opengl-objects/shader.h"
#include "opengl-objects/texture.h"

#include <GL/gl.h>
#include <chrono>
#include <cmath>
#include <iterator>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;

GLuint VAO, VBO, FBO, RBO;

const char *vsSource;
const char *fsSource;
const char *csSource;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test Window", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGuiWindowFlags wFlags;
  // wFlags |= ImGuiWindowFlags_NoTitleBar;
  wFlags |= ImGuiWindowFlags_NoResize;
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

  Framebuffer framebuffer(&texFB);

  GLuint vbo, vao;

  GLuint imgOutputLocation = computeShader.getUniformLocation("imgOutput");
  GLuint texLocation = shader.getUniformLocation("tex");

  // shader.use();
  // createBox(vbo, vao);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    // processInput(window);

    auto startTime = std::chrono::system_clock::now();
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    framebuffer.bindFramebuffer();
    ImGui::NewFrame();
    {
      ImGui::Begin("Scene");
      const float windowWidth = ImGui::GetContentRegionAvail().x;
      const float windowHeight = ImGui::GetContentRegionAvail().y;

      framebuffer.rescaleFramebuffer(windowWidth, windowHeight);
      glViewport(0, 0, windowWidth, windowHeight);

      ImVec2 pos = ImGui::GetCursorScreenPos();

      ImGui::Image((ImTextureID)(intptr_t)texCS.texture,
                   ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }

    ImGui::SetNextWindowSize(ImVec2(100, 200));
    ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
    {
      // Create Buttons window
      ImGui::Begin("Buttons" /* , NULL, wFlags */);
      ImGui::Button("Hello");
      ImGui::End();
    }

    ImGui::Render();
    framebuffer.unbindFramebuffer();

    texCS.bindTexture();
    computeShader.use();
    glUniform1i(imgOutputLocation, texCS.texNum);
    glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT,
                      1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texCS.unbindTexture();
    glUniform1i(texLocation, texCS.texNum);

    shader.use();
    renderQuad(vao);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backupCurrentContext = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backupCurrentContext);
    }

    glfwSwapBuffers(window);
    auto endTime = std::chrono::system_clock::now();
    std::cout << (endTime - startTime).count() << std::endl;
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
  glViewport(0, 0, width, height);
}
