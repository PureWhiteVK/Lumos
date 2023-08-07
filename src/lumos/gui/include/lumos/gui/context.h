#include "lumos/gui/common.h"

#include <functional>

struct GLFWwindow;

namespace lumos {
namespace gui {
class Context {
public:
  static Context &instance();

  void initialize(const char *window_title, int default_width,
                  int default_height);

  void loop(const std::function<void()> &render_func = []() {});

  operator GLFWwindow *() const { return m_window; }

  ~Context();

private:
  Context() = default;

  static void glfwErrorCallback(int error, const char *description);
  static void glfwResizeCallback(GLFWwindow *window, int width, int height);

private:
  GLFWwindow *m_window{};
  int m_width{}, m_height{};
};
} // namespace gui
} // namespace lumos