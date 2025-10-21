#include "Window.hpp"
#include "GLFW/glfw3.h"
#include "ImGuiFileDialog.h"
#include "MatrixUtils.hpp"
#include "Parser.hpp"
#include "Renderer.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "models/Blade.hpp"
#include <GL/gl.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

Window::Window(uint16_t width, uint16_t height, std::string title)
    : m_camera(1.f, {0.0f, 0.0f, 0.0f}), m_t(0.f), m_height(height),
      m_width(width), m_clicked(false) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  m_window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(
      glfwCreateWindow(width, height, title.c_str(), NULL, NULL));
  if (m_window.get() == NULL) {
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(m_window.get());
  glewInit();

  m_renderer = std::make_unique<Renderer>();
  m_scene = std::make_unique<Scene>();

  m_renderer->setProjection(math137::MatrixUtils::Projection(
      M_PI_4f, (float)m_width / m_height, 0.1f, 100.f));

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui_ImplGlfw_InitForOpenGL(m_window.get(), true);
  ImGui_ImplOpenGL3_Init();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glfwSetWindowUserPointer(m_window.get(), reinterpret_cast<void *>(this));
  glfwSetScrollCallback(m_window.get(), scrollInputCallback);
  glfwSetKeyCallback(m_window.get(), keyInputCallback);
  glfwSetMouseButtonCallback(m_window.get(), mouseButtonCallback);
  glfwSetCursorPosCallback(m_window.get(), cursorPositionCallback);
  glfwSetFramebufferSizeCallback(m_window.get(), resizeWindowCallback);
}

Window::~Window() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
}

void Window::update(bool &running) {
  running = !glfwWindowShouldClose(m_window.get());
  glfwPollEvents();

  m_scene->update();
}

void Window::draw() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float t = glfwGetTime();
  m_renderer->setView(m_camera.getView());
  m_renderer->setCamerPos(m_camera.getPosition());
  m_scene->render(m_renderer);
  renderImgui(t - m_t);
  glfwSwapBuffers(m_window.get());
  m_t = t;
}

void Window::renderImgui(float dt) {
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(100, m_height), ImGuiCond_Always);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if (ImGui::Button("Load G-Code")) {
    IGFD::FileDialogConfig config;
    config.path = "../paths/";
    ImGuiFileDialog::Instance()->OpenDialog("ImportFile", "Import File", ".*",
                                            config);
  }
  ImGui::Separator();
  static float bladeR = 1.f;
  static float bladeHeight = 20.f;
  ImGui::SliderFloat("Cutter Radius", &bladeR, 0.5f, 20.f);
  ImGui::SliderFloat("Cutter height", &bladeHeight, 1.f, 40.f);
  static const char *cutterType[] = {"Flat", "Ball"};
  static int selected = 0;
  ImGui::Combo("Cutter type", &selected, cutterType, IM_ARRAYSIZE(cutterType));
  if (ImGui::Button("Set cutter")) {
    m_scene->setBladeData(bladeR, bladeHeight,
                          static_cast<BladeType>(selected));
  }
  ImGui::Separator();
  static int resX = 600, resZ = 600;
  static float sx = 15.f, sy = 5.f, sz = 15.f, base = 15.f;
  ImGui::InputInt("Divisons X", &resX);
  ImGui::InputInt("Divisons Z", &resZ);
  ImGui::SliderFloat("Size X [cm]", &sx, 1.f, 20.f);
  ImGui::SliderFloat("Size Y [cm]", &sy, 1.f, 10.f);
  ImGui::SliderFloat("Size Z [cm]", &sz, 1.f, 20.f);
  ImGui::SliderFloat("Base height [mm]", &base, 1.f, 20.f);
  if (ImGui::Button("Set block")) {
    m_scene->setBlockData(resX, resZ, sx, sy, sz, base);
  }
  ImGui::Separator();
  static int speed = 10;
  static float angle = M_PI_4f;
  if (ImGui::InputInt("speed", &speed)) {
    m_scene->setSimulatorSpeed(speed);
  }
  if (ImGui::SliderFloat("MaxAngle", &angle, 0.0f, M_PI_2f)) {
    m_scene->setMaxAngle(angle);
  }
  if (ImGui::Button("Start simulation")) {
    m_scene->startSimulator();
  }
  ImGui::SameLine();
  if (ImGui::Button("Finish simulation")) {
    m_scene->finishSimulator();
  }
  ImGui::Text("%d frames", (int)(1 / dt));
  if (ImGuiFileDialog::Instance()->Display("ImportFile")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
      std::ifstream input(filePath);
      std::stringstream ss;
      ss << input.rdbuf();
      auto res = Parser::ParseGCode(std::move(ss.str()));
      m_scene->setSimulatorMoves(res);
      std::pair<BladeType, float> tf = Parser::ParseFileName(
          ImGuiFileDialog::Instance()->GetCurrentFileName());
      bladeR = tf.second;
      selected = static_cast<int>(tf.first);
      m_scene->setBladeRadius(tf.second);
      m_scene->setBladeType(tf.first);
    }

    ImGuiFileDialog::Instance()->Close();
  }
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::keyInputCallback(GLFWwindow *window, int key, int scancode,
                              int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::cursorPositionCallback(GLFWwindow *window, double xpos,
                                    double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
  Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  static double x, y;
  if (w->m_clicked)
    w->m_camera.rotateCamera(x - xpos, y - ypos);
  x = xpos;
  y = ypos;
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action,
                                 int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  w->m_clicked = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS);
}

void Window::scrollInputCallback(GLFWwindow *window, double xOffset,
                                 double yOffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  w->m_camera.changeDistance(0.08f * yOffset);
}
void Window::resizeWindowCallback(GLFWwindow *window, int width, int height) {}
