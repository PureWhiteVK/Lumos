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
#include "lumos/gui/common.h"
#include "lumos/gui/context.h"
#include "lumos/gui/handle.h"
#include "lumos/gui/imgui_utils.h"
#include "lumos/gui/texture.h"

#include <chrono>
#include <exception>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <nfd.hpp>

namespace fs = std::filesystem;

using ImageLoaderMap = std::unordered_map<
    std::string, std::function<void(const fs::path &, lumos::ImageData4u8 *)>>;

// Main code
int main() {
  try {
    auto logger = lumos::setupLogger({});
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    lumos::gui::Context &context = lumos::gui::Context::instance();

    context.initialize("Test Viewer", 1280, 720);

    ImGuiIO &io = ImGui::GetIO();
    ImageLoaderMap image_loader_map{
        {".png", lumos::readPng},
        {".exr", lumos::readExr<lumos::ImageData4u8>}};

    // Our state
    bool show_preview_window = false;
    bool show_demo_window = true;
    bool show_image_tooltip = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::optional<fs::path> preview_image_path;
    lumos::ImageData4u8 image_data;
    lumos::gui::Texture texture;

    auto on_image_button_click = [&]() {
      static nfdnchar_t *out_path;
      static nfdfilteritem_t filterItem[2] = {
          {"Portable Network Graphics", "png"}, {"OpenEXR", "exr"}};
      nfdresult_t nfd_result = NFD::OpenDialog(out_path, filterItem, 2);
      switch (nfd_result) {
      case NFD_OKAY:
        DEBUG("user select path: {}", out_path);
        preview_image_path = fs::path(out_path);
        break;
      case NFD_ERROR:
        throw lumos::RuntimeError("NFD error: {}", NFD::GetError());
      case NFD_CANCEL:
        DEBUG("operation canceled.");
        break;
      }
      if (!preview_image_path) {
        return;
      }
      // should we use another thread to load image ?
      show_preview_window = true;
      // load image here
      std::string ext = preview_image_path->extension().string();
      DEBUG("image extension: {}", ext);
      if (image_loader_map.find(ext) == image_loader_map.end()) {
        throw lumos::RuntimeError("unsupported image extension: {}", ext);
      }
      image_loader_map[ext](preview_image_path.value(), &image_data);
      if (!texture || image_data.rows() != texture.height() ||
          image_data.cols() != texture.width()) {
        DEBUG("texture size change!, reallocate texture");
        texture.resize(image_data.cols(), image_data.rows());
      }
      texture.update(image_data);
    };

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);

    auto render_func = [&]() {
      // actually we will handle user input here?
      if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        glfwSetWindowShouldClose(context, true);
      }

      // # Begin UI render
      static ImGuiID dockspace_id;
      static ImGuiID left_id{};
      static ImGuiID right_id{};

      dockspace_id = ImGui::DockSpaceOverViewport();

      if (!ImGui::DockBuilderGetNode(dockspace_id)->IsSplitNode()) {
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, &left_id,
                                    &right_id);
        ImGui::DockBuilderGetNode(left_id)->SetLocalFlags(
            ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar);
        ImGui::DockBuilderFinish(dockspace_id);
      }

      if (show_demo_window) {
        ImGui::SetNextWindowDockID(right_id);
        ImGui::ShowDemoWindow(&show_demo_window);
      }

      // Control Panel
      {
        ImGui::SetNextWindowDockID(left_id);
        ImGui::Begin("Control Panel");
        ImGui::Checkbox("Show Demo Window", &show_demo_window);
        if (ImGui::ColorEdit3("Clear color", (float *)&clear_color)) {
          DEBUG("update clear color");
          glClearColor(clear_color.x * clear_color.w,
                       clear_color.y * clear_color.w,
                       clear_color.z * clear_color.w, clear_color.w);
        }
        if (ImGui::Button("Open image")) {
          on_image_button_click();
        }
        if (preview_image_path.has_value()) {
          ImGui::SameLine();
          ImGui::Text("Image: %s", preview_image_path->filename().c_str());
        }
        ImGui::Checkbox("Zoom in", &show_image_tooltip);
        ImGui::Separator();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
      }

      if (show_preview_window) {
        static ImVec2 window_size, window_padding, image_size, pos,
            relative_pos;
        static ImVec4 tint_color{1, 1, 1, 1};
        static ImVec4 boarder_color = ImGui::GetStyleColorVec4(ImGuiCol_Border);
        static ImTextureID texture_id;

        // 仅设置一次，这样我们可以重新调整窗口的位置，但是有一个初始的
        // docking 位置
        ImGui::SetNextWindowDockID(right_id);
        ImGui::Begin(preview_image_path->filename().c_str(),
                     &show_preview_window);
        window_size = ImGui::GetWindowSize();
        window_padding = ImGui::GetStyle().WindowPadding;

        image_size.x = std::min(window_size.x - 2.0f * window_padding.x,
                                static_cast<float>(texture.width()));

        image_size.y = static_cast<float>(texture.height()) /
                       static_cast<float>(texture.width()) * image_size.x;
        texture_id =
            reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture));
        pos = ImGui::GetCursorScreenPos();
        ImGui::Image(texture_id, image_size, {0, 0}, {1, 1}, tint_color,
                     boarder_color);
        relative_pos.x = io.MousePos.x - pos.x;
        relative_pos.y = io.MousePos.y - pos.y;
        if (show_image_tooltip) {
          lumos::gui::imageTooltip(texture_id, relative_pos, image_size,
                                   tint_color, boarder_color);
        }
        ImGui::End();
      } else {
        if (preview_image_path.has_value()) {
          DEBUG("deselect image");
          preview_image_path.reset();
        }
      }
    };

    context.loop(render_func);
    return 0;
  } catch (std::exception &e) {
    ERROR("Exception: {}", e.what());
    return 1;
  }
}
