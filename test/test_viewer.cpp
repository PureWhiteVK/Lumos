// Dear ImGui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline (GLFW is a cross-platform general purpose library for
// handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation,
// etc.) If you are new to Dear ImGui, read documentation from the docs/ folder
// + read the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

#include "lumos/core/color.h"
#include "lumos/core/common.h"
#include "lumos/core/exception.h"
#include "lumos/core/imageio.h"

#include <exception>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.hpp>
#include <spdlog/fmt/bundled/format.h>

namespace fs = std::filesystem;

static void glfw_error_callback(int error, const char *description) {
  ERROR("GLFW Error {}: {}", error, description);
}

fs::path get_user_image_path() {
  static nfdnchar_t *out_path;
  static nfdfilteritem_t filterItem[2] = {{"Portable Network Graphics", "png"},
                                          {"OpenEXR", "exr"}};
  auto res = NFD::OpenDialog(out_path, filterItem, 2);
  switch (res) {
  case NFD_OKAY:
    DEBUG("user select path: {}", out_path);
    break;
  case NFD_ERROR:
    throw lumos::runtime_error("nfd error!");
  case NFD_CANCEL:
    DEBUG("user canceled.");
    break;
  }
  return fs::path(out_path);
}

using ContextHandle =
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *)>>;

using TextureHandle = std::unique_ptr<GLuint, std::function<void(GLuint *)>>;

using ImageLoaderMap =
    std::unordered_map<std::string,
                       std::function<lumos::ImageData4u8(const fs::path &)>>;

void deinit_context(GLFWwindow *window) {
  DEBUG("deinit context");
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  NFD_Quit();
}

ContextHandle init_context() {
  auto logger = lumos::setupLogger({});
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  DEBUG("init context");
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw lumos::runtime_error("failed to initialize GLFW.");
  }

  const char *glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(
      1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
  if (window == nullptr) {
    throw lumos::runtime_error("failed to create GLFW window.");
  }

  ContextHandle window_handle{window, deinit_context};

  glfwMakeContextCurrent(window);

  if (!gladLoadGL(glfwGetProcAddress)) {
    throw lumos::runtime_error("failed to load GL context.");
  }

  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
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
  // ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // io.Fonts->AddFontDefault();
  fs::path font_path = lumos::getDataPath("LXGWNeoXiHeiScreen.ttf");
  ImFont *font = io.Fonts->AddFontFromFileTTF(
      font_path.c_str(), 14.0f, nullptr,
      io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  if (!font) {
    throw lumos::runtime_error("failed to load font: {}", font_path.c_str());
  }
  // init nfd
  if (NFD::Init() != NFD_OKAY) {
    throw lumos::runtime_error("failed to initialize Native File Dialog.");
  }
  return window_handle;
}

void destory_texture(GLuint *texture_ptr) {
  DEBUG("destory texture[{:#0x}]", *texture_ptr);
  glBindTexture(GL_TEXTURE_2D, *texture_ptr);
  glDeleteTextures(1, texture_ptr);
  delete texture_ptr;
}

TextureHandle create_texture(int width, int height) {
  GLuint *texture_ptr = new GLuint{};
  glGenTextures(1, texture_ptr);
  DEBUG("create texture[{:#0x}], hxw: {}x{}", *texture_ptr, height, width);
  glBindTexture(GL_TEXTURE_2D, *texture_ptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  return TextureHandle(texture_ptr, destory_texture);
}

void upload_texture(const TextureHandle &texture_handle,
                    const lumos::ImageData4u8 &image_data) {
  DEBUG("upload texture[{:#0x}], hxw: {}x{}", *texture_handle,
        image_data.rows(), image_data.cols());
  glBindTexture(GL_TEXTURE_2D, *texture_handle);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_data.cols(), image_data.rows(),
                  GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());
}

// Main code
int main() {
  try {
    ContextHandle handle = init_context();
    ImGuiIO &io = ImGui::GetIO();

    ImageLoaderMap image_loader_map{
        {".png", lumos::readPng},
        {".exr", [](const fs::path &p) {
           lumos::ImageData4u8 image_data = lumos::readExr(p).unaryExpr(
               [](auto c) { return lumos::toUint8(lumos::toSrgb(c)); });
           DEBUG("convert exr file: {}", p.c_str());
           return image_data;
         }}};

    // Our state
    bool show_preview_window = false;
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::optional<fs::path> preview_image_path;
    lumos::ImageData4u8 image_data;

    int texture_height = 1, texture_width = 1;
    TextureHandle texture_handle;

    // create image_texture

    while (!glfwWindowShouldClose(handle.get())) {
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

      // # Begin UI render
      static ImGuiID root_node_id;
      root_node_id = ImGui::DockSpaceOverViewport();

      // Main menu bar
      {
        if (ImGui::BeginMainMenuBar()) {
          if (ImGui::BeginMenu("Options")) {
            ImGui::MenuItem("Padding", nullptr, nullptr);
            ImGui::EndMenu();
          }
          if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", nullptr)) {
            }
            ImGui::EndMenu();
          }
          if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("Padding", nullptr, nullptr);
            ImGui::EndMenu();
          }
        }
        ImGui::EndMainMenuBar();
      }

      if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
      }

      // Control Panel
      {
        ImGui::Begin("Control Panel");
        ImGui::Checkbox("Demo Window",
                        &show_demo_window); // Edit bools storing our window
                                            // open/close state
        ImGui::ColorEdit3(
            "clear color",
            (float *)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("open image")) {
          fs::path image_path = get_user_image_path();
          // should we use another thread to load image ?
          preview_image_path = image_path;
          show_preview_window = true;
          // load image here
          auto ext = image_path.extension().string();
          DEBUG("image extension: {}", ext);
          if (image_loader_map.find(ext) == image_loader_map.end()) {
            throw lumos::runtime_error("unsupported image extension: {}", ext);
          }
          image_data = image_loader_map[ext](image_path);
          if (!texture_handle || image_data.rows() != texture_height ||
              image_data.cols() != texture_width) {
            DEBUG("texture size change!, reallocate texture");
            texture_height = image_data.rows();
            texture_width = image_data.cols();
            texture_handle = create_texture(texture_width, texture_height);
          }
          upload_texture(texture_handle, image_data);
        }
        if (preview_image_path.has_value()) {
          ImGui::SameLine();
          ImGui::Text("Image: %s", preview_image_path->filename().c_str());
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
      }

      if (show_preview_window) {
        ImGui::Begin("image preview", &show_preview_window,
                     ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("%s", preview_image_path->filename().c_str());
        // we have to scale the image so the image will fit current window size
        ImGui::Image(reinterpret_cast<ImTextureID>(
                         static_cast<intptr_t>(*texture_handle)),
                     {static_cast<float>(texture_width),
                      static_cast<float>(texture_height)});
        ImGui::End();
      } else {
        if (preview_image_path.has_value()) {
          DEBUG("deselect image");
          preview_image_path.reset();
        }
      }

      // Rendering
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(handle.get(), &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                   clear_color.z * clear_color.w, clear_color.w);
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
      glfwSwapBuffers(handle.get());
    }
    return 0;
  } catch (std::exception &e) {
    ERROR("Exception: {}", e.what());
    return 1;
  }
}
