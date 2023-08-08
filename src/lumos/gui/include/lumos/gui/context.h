#pragma once

#include "lumos/gui/common.h"
#include <functional>

struct GLFWwindow;

namespace lumos {
namespace gui {
class Context {
public:
  static Context &Instance();

  void Initialize(const char *window_title = "ImGui + OpenGL3 + GLFW (Window)",
                  int default_width = 1280, int default_height = 720);

  void Loop(const std::function<void()> &render_func = defaultRenderFunc);

  int width() const { return m_width; }
  int height() const { return m_height; }
  operator GLFWwindow *() const { return m_window; }

  Context(const Context &) = delete;
  Context(Context &&) = delete;
  Context &operator=(const Context &) = delete;
  Context &operator=(Context &&) = delete;

  ~Context();

private:
  Context() = default;

  static void glfwErrorCallback(int error, const char *description);
  static void glfwResizeCallback(GLFWwindow *window, int width, int height);
  static void defaultRenderFunc();

private:
  GLFWwindow *m_window{};
  int m_width{}, m_height{};
};
} // namespace gui
} // namespace lumos