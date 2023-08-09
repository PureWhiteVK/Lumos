#include "lumos/gui/context.h"
#include "lumos/core/exception.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.hpp>

namespace fs = std::filesystem;

namespace lumos {
namespace gui {

void Context::glfwErrorCallback(int error, const char *description) {
  throw RuntimeError("GLFW Error {}: {}", error, description);
}

void Context::defaultRenderFunc() {
  ImGui::ShowDemoWindow();
}

Context &Context::Instance() {
  static Context context;
  return context;
}

void Context::Initialize(const char *window_title, int default_width,
                         int default_height) {
  m_height = default_height;
  m_width = default_width;
  DEBUG("create context");
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    throw lumos::RuntimeError("failed to initialize GLFW.");
  }

  const char *glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  m_window =
      glfwCreateWindow(m_width, m_height, window_title, nullptr, nullptr);

  if (m_window == nullptr) {
    throw lumos::RuntimeError("failed to create GLFW window.");
  }

  glfwMakeContextCurrent(m_window);

  if (!gladLoadGL(glfwGetProcAddress)) {
    throw lumos::RuntimeError("failed to load GL context.");
  }

  glfwSwapInterval(1);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;              // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport
                                                      // / Platform Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // disable ini layout file
  io.IniFilename = nullptr;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_DEPTH_TEST);
  // io.Fonts->AddFontDefault();
  fs::path font_path = lumos::GetDataPath("LXGWNeoXiHeiScreen.ttf");
  ImFont *font = io.Fonts->AddFontFromFileTTF(
      font_path.c_str(), 14.0f, nullptr,
      io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  if (!font) {
    throw lumos::RuntimeError("failed to load font: {}", font_path.c_str());
  }
  // init nfd
  if (NFD::Init() != NFD_OKAY) {
    throw lumos::RuntimeError("failed to initialize Native File Dialog.");
  }
}

Context::~Context() {
  DEBUG("destroy context");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(m_window);
  glfwTerminate();
  NFD::Quit();
}

void Context::Loop(const std::function<void()> &render_func) {
  DEBUG("start render loop");
  const ImGuiIO &io = ImGui::GetIO();
  while (!glfwWindowShouldClose(m_window)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    render_func();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we
    // save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call
    //  glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(m_window);
  }
}
} // namespace gui
} // namespace lumos