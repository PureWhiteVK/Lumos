
#include "lumos/core/common.h"
#include "lumos/core/exception.h"
#include "lumos/gui/common.h"
#include "lumos/gui/context.h"
#include "lumos/gui/framebuffer.h"
#include "lumos/gui/handle.h"
#include "lumos/gui/shader.h"
#include "lumos/gui/texture.h"

#include <exception>

#include <Eigen/Dense>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
// #include <Eigen/Geometry>

/*
TODO:
2. get rid of glm (switching to eigen!)
# 需要将纯相机类和相机交互类分离开？，gui部分实现交互，然后 core
部分就是纯相机，包含 view 和 projection matrix？
3. camera class implementation (offline and online?)
*/

namespace gui = lumos::gui;

// N_faces, 3 matrix
using IndexBuffer = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;
// N_vertices, 3 / 6
using VertexBuffer =
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

// using mat4 = Eigen::Matrix4f;
// using vec3 = Eigen::Vector3f;
// using vec2 = Eigen::Vector2f;
// using quad = Eigen::Quaternionf;

template <>
void gui::SetUniform(GLuint program, std::string name, const glm::mat4 &data) {
  GLint location = glGetUniformLocation(program, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
  CheckOpenGLErrors();
}

/*
 * A simple arcball camera that moves around the camera's focal point.
 * The mouse inputs to the camera should be in normalized device coordinates,
 * where the top-left of the screen corresponds to [-1, 1], and the bottom
 * right is [1, -1].
 */
class ArcballCamera {
public:
  glm::mat4 cameraTranslation{1.0f};
  glm::mat4 rotation{1.0f};
  /*
   * 1. translate the desire object center to (0,0,0)
   * 2. rotate the object
   * 3. translate the camera to desire position
   * combine 3 steps together, we got the view matrix
   * view = cameraTranslation * rotation * cameraTranslation
   * */
  glm::mat4 view{1.0f};

  /*
   * viewInverse = glm::mat4{
   *      glm::vec4{u,0.0f},
   *      glm::vec4{v,0.0f},
   *      glm::vec4{w,0.0f},
   *      glm::vec4{e,1.0f}
   * }
   * // 其中 e = glm::vec3{-x_e,-y_e,-z_e}
   * */
  glm::mat4 viewInverse{1.0f};

  float zoomRatio = 1.0f;

  float zoomMin = 0.01f;
  float zoomMax = 100.0f;

public:
  /*
   * Create an arcball camera focused on some center point
   * screen: [win_width, win_height]
   */
  ArcballCamera(const glm::vec3 &eye, const glm::vec3 &up) {
    // 此时相机的up和eye-objectCenter不一定是垂直了，需要重新进行计算
    // 相机看向的是z的负向，需要调整
    // actually it's eye - origin(0.0f,0.0f,0.0f)
    glm::vec3 translateDir = eye;
    float translateDist = glm::length(translateDir);
    glm::vec3 z = glm::normalize(translateDir);
    glm::vec3 x = glm::cross(glm::normalize(up), z);
    glm::vec3 y = glm::cross(z, x);
    // 沿视线方向移动glm::length(cameraInverseDir)距离
    // zoom实际上就是改变这个距离，也就是相机距离观测点之间的直线距离
    cameraTranslation = glm::mat4{glm::vec4{1.0f, 0.0f, 0.0f, 0.0f},
                                  glm::vec4{0.0f, 1.0f, 0.0f, 0.0f},
                                  glm::vec4{0.0f, 0.0f, 1.0f, 0.0f},
                                  glm::vec4{0.0f, 0.0f, -translateDist, 1.0f}};

    rotation = glm::transpose(glm::mat4{glm::vec4{x, 0.0f}, glm::vec4{y, 0.0f},
                                        glm::vec4{z, 0.0f},
                                        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}});

    updateViewMatrix();
  }

  ArcballCamera() = default;

  ArcballCamera(const ArcballCamera &c) = default;

  ArcballCamera &operator=(const ArcballCamera &c) = default;

  /* Rotate the camera from the previous mouse position to the current
   * one. Mouse positions should be in normalized device coordinates
   */
  // 旋转
  void Rotate(const glm::vec2 &prevCursor, const glm::vec2 &currCursor) {
    glm::quat p1 = NDCtoUnitSphereType2(prevCursor);
    glm::quat p2 = NDCtoUnitSphereType2(currCursor);
    // target = [0,e1]
    // p2 = [0,e2]
    // target*p2 = [-glm::dot(e1,e2),glm::cross(e1,e2)]
    // 直观上理解就是将物体绕单位球 过 target、p2、O平面 的法线 旋转
    // 2*arccos(glm::dot(e1,e2))
    glm::mat4 rotate = glm::mat4_cast(p2 * p1);
    rotation = rotate * rotation;
    updateViewMatrix();
  }

  /*
   * Zoom the camera given the zoom amount to (i.e., the scroll amount).
   * Positive values zoom in, negative will zoom out.
   */
  void Zoom(float amount) {
    // Zoom actually make the camera closer /  to the object
    // 调整相机距离amount的距离
    float dist = GetCameraDistance();
    // 每次放大 1.0f + amount 倍
    cameraTranslation[3][2] =
        -glm::clamp(dist + zoomRatio * amount, zoomMin, zoomMax);
    updateViewMatrix();
  }

  float GetCameraDistance() const { return -cameraTranslation[3][2]; }

  // 变换矩阵
  // Get the camera transformation matrix
  const glm::mat4 &GetViewMatrix() const { return view; }

  // Get the camera's inverse transformation matrix
  const glm::mat4 &GetViewInverseMatrix() const { return viewInverse; }

  // Get the eye position of the camera in world space
  glm::vec3 GetCameraPosition() const {
    // 实际上就是 viewInverse * glm::vec4{0.0f,0.0f,0.0f,1.0f}
    return -glm::vec3{viewInverse[3]};
  }

  // Get the eye direction of the camera in world space
  glm::vec3 GetFrontDirection() const { return -glm::vec3{viewInverse[2]}; }

  // Get the up direction of the camera in world space
  glm::vec3 GetUpDirection() const { return glm::vec3{viewInverse[1]}; }

  // Get the right direction of the camera in world space
  glm::vec3 GetRightDirection() const { return glm::vec3{viewInverse[0]}; }

private:
  void updateViewMatrix() {
    view = cameraTranslation * rotation;
    viewInverse =
        inverseRotation(rotation) * inverseTranslate(cameraTranslation);
  }

  static glm::quat NDCtoUnitSphereType1(const glm::vec2 &p) {
    float t = 2.0f / (1.0f + glm::dot(p, p));
    return {0.0f, t * p.x, t * p.y, t - 1};
  }

  static glm::quat NDCtoUnitSphereType2(const glm::vec2 &p) {
    const float dist = glm::dot(p, p);
    if (dist <= 1.f) {
      return {0.0, p.x, p.y, std::sqrt(1.f - dist)};
    } else {
      const glm::vec2 proj = glm::normalize(p);
      return {0.0, proj.x, proj.y, 0.f};
    }
  }

  static glm::mat4 inverseTranslate(const glm::mat4 &m) {
    return glm::mat4{m[0], m[1], m[2],
                     glm::vec4{-m[3].x, -m[3].y, -m[3].z, 1.0f}};
  }

  static glm::mat4 inverseRotation(const glm::mat4 &m) {
    return glm::transpose(m);
  }
};

int main() {
  try {
    lumos::SetupLogger({});
    spdlog::set_level(spdlog::level::debug);
    auto &instance = gui::Context::Instance();
    instance.Initialize();

    // TODO: 再画一个 xz 平面，对比 screen space 重建出来的效果

    IndexBuffer cube_indices{
        {0, 1, 2},    {2, 3, 0},    // v0-v1-v2, v2-v3-v0 (front)
        {4, 5, 6},    {6, 7, 4},    // v0-v3-v4, v4-v5-v0 (right)
        {8, 9, 10},   {10, 11, 8},  // v0-v5-v6, v6-v1-v0 (top)
        {12, 13, 14}, {14, 15, 12}, // v1-v6-v7, v7-v2-v1 (left)
        {16, 17, 18}, {18, 19, 16}, // v7-v4-v3, v3-v2-v7 (bottom)
        {20, 21, 22}, {22, 23, 20}, // v4-v7-v6, v6-v5-v4 (back)
    };

    VertexBuffer cube_vertices{
        // positions (x,y,z) normal (x,y,z)
        {.5f, .5f, .5f, 0.0f, 0.0f, 1.0f},
        {-.5f, .5f, .5f, 0.0f, 0.0f, 1.0f},
        {-.5f, -.5f, .5f, 0.0f, 0.0f, 1.0f},
        {.5f, -.5f, .5f, 0.0f, 0.0f, 1.0f}, // v0,v1,v2,v3 (front)
        {.5f, .5f, .5f, 1.0f, 0.0f, 0.0f},
        {.5f, -.5f, .5f, 1.0f, 0.0f, 0.0f},
        {.5f, -.5f, -.5f, 1.0f, 0.0f, 0.0f},
        {.5f, .5f, -.5f, 1.0f, 0.0f, 0.0f}, // v0,v3,v4,v5 (right)
        {.5f, .5f, .5f, 0.0f, 1.0f, 0.0f},
        {.5f, .5f, -.5f, 0.0f, 1.0f, 0.0f},
        {-.5f, .5f, -.5f, 0.0f, 1.0f, 0.0f},
        {-.5f, .5f, .5f, 0.0f, 1.0f, 0.0f}, // v0,v5,v6,v1 (top)
        {-.5f, .5f, .5f, -1.0f, 0.0f, 0.0f},
        {-.5f, .5f, -.5f, -1.0f, 0.0f, 0.0f},
        {-.5f, -.5f, -.5f, -1.0f, 0.0f, 0.0f},
        {-.5f, -.5f, .5f, -1.0f, 0.0f, 0.0f}, // v1,v6,v7,v2 (left)
        {-.5f, -.5f, -.5f, 0.0f, -1.0f, 0.0f},
        {.5f, -.5f, -.5f, 0.0f, -1.0f, 0.0f},
        {.5f, -.5f, .5f, 0.0f, -1.0f, 0.0f},
        {-.5f, -.5f, .5f, 0.0f, -1.0f, 0.0f}, // v7,v4,v3,v2 (bottom)
        {.5f, -.5f, -.5f, 0.0f, 0.0f, -1.0f},
        {-.5f, -.5f, -.5f, 0.0f, 0.0f, -1.0f},
        {-.5f, .5f, -.5f, 0.0f, 0.0f, -1.0f},
        {.5f, .5f, -.5f, 0.0f, 0.0f, -1.0f}, // v4,v7,v6,v5 (back)
    };

    std::filesystem::path shader_path = std::filesystem::u8path("中文shader");

    gui::DrawProgram cube_shader(lumos::GetDataPath(shader_path / "cube.vert"),
                                 lumos::GetDataPath(shader_path / "cube.frag"));

    gui::VertexArrayHandle cube_vao;
    gui::BufferHandle cube_vbo, cube_ebo;
    {
      glGenVertexArrays(1, cube_vao.Ptr());
      glGenBuffers(1, cube_vbo.Ptr());
      glGenBuffers(1, cube_ebo.Ptr());

      glBindVertexArray(cube_vao);
      glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(decltype(cube_vertices)::Scalar) *
                       cube_vertices.size(),
                   cube_vertices.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(decltype(cube_indices)::Scalar) * cube_indices.size(),
                   cube_indices.data(), GL_STATIC_DRAW);
      // we should set this value according to vertex value
      // vertex positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            sizeof(decltype(cube_vertices)::Scalar) * 6,
                            (void *)0);
      // vertex normals
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(
          1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(cube_vertices)::Scalar) * 6,
          (void *)(sizeof(decltype(cube_vertices)::Scalar) * 3));
      // vertex texture coords
      // glEnableVertexAttribArray(2);
      // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
      //                       (void *)offsetof(Vertex, TexCoords));

      glBindVertexArray(0);
    }

    gui::DrawProgram plane_shader(
        lumos::GetDataPath(shader_path / "plane.vert"),
        lumos::GetDataPath(shader_path / "plane.frag"));

    gui::VertexArrayHandle plane_vao;
    {
      // generate empty plane_vao (to clear out data bind)
      glGenVertexArrays(1, plane_vao.Ptr());
    }

    // view
    ArcballCamera camera = ArcballCamera(glm::vec3{0.0f, 0.0f, -10.0f},
                                         glm::vec3{0.0f, 1.0f, 0.0f});

    // model will change with time?
    ImGuiIO &io = ImGui::GetIO();
    ImVec4 c{0.3f, 0.2f, 0.1f, 1.0f};

    glm::vec3 center{0.0f, 0.0f, 0.0f};

    const unsigned int CENTER_X = instance.Width() / 2;
    const unsigned int CENTER_Y = instance.Height() / 2;

    ImVec2 window_pos;
    ImVec2 prev_pos;
    ImVec2 curr_pos;

    auto cursorPosToNDCCoord = [=](const ImVec2 &pos) -> glm::vec2 {
      return {lumos::Clamp((pos.x - CENTER_X) / CENTER_X, -1.0f, 1.0f),
              lumos::Clamp((pos.y - CENTER_Y) / CENTER_Y, -1.0f, 1.0f)};
    };

    bool activate_arcball = false;
    bool item_hovered = false;
    bool draw_cube = false;
    bool draw_plane = false;

    auto v = camera.view * camera.viewInverse;

    glEnable(GL_DEPTH_TEST);

    int framebuffer_width = 800;
    int framebuffer_height = 600;
    // gui::FrameBuffer frame_buffer(framebuffer_width, framebuffer_height,
    //                               GL_DEPTH_COMPONENT24);

    // projection
    glm::mat4 projection =
        glm::perspective(glm::radians<float>(45.0f),
                         static_cast<float>(framebuffer_width) /
                             static_cast<float>(framebuffer_height),
                         0.1f, 100.0f);
    glm::mat4 projection_inv = glm::inverse(projection);

    int msaa_samples_bits = 0;

    gui::MsaaFrameBuffer msaa_frame_buffer(
        framebuffer_width, framebuffer_height, GL_DEPTH_COMPONENT16,
        1 << msaa_samples_bits);
    gui::FrameBuffer final_frame_buffer(framebuffer_width, framebuffer_height);

    auto render = [&]() {
      // IO
      if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        glfwSetWindowShouldClose(instance, true);
      }

      // RENDER TO TEXTURE
      {
        glBindFramebuffer(GL_FRAMEBUFFER, msaa_frame_buffer);
        glClearColor(c.x, c.y, c.z, c.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, framebuffer_width, framebuffer_height);

        // draw normal cube here
        if (draw_cube) {
          glUseProgram(cube_shader);
          glBindVertexArray(cube_vao);
          glm::mat4 model{1.0f};
          gui::SetUniform(cube_shader, "mvp",
                          projection * camera.GetViewMatrix() * model);
          glDrawElements(GL_TRIANGLES, cube_indices.size(), GL_UNSIGNED_INT,
                         nullptr);
        }

        // draw plane here?
        if (draw_plane) {
          glUseProgram(plane_shader);
          glBindVertexArray(plane_vao);
          gui::SetUniform(plane_shader, "view", camera.GetViewMatrix());
          gui::SetUniform(plane_shader, "proj", projection);
          gui::SetUniform(plane_shader, "view_inv",
                          camera.GetViewInverseMatrix());
          gui::SetUniform(plane_shader, "proj_inv", projection_inv);
          glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // blit result to final frame for post processing

        glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, final_frame_buffer);
        glBlitFramebuffer(0, 0, framebuffer_width, framebuffer_height, 0, 0,
                          framebuffer_width, framebuffer_height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
      }

      // create dockspace

      // IMGUI
      ImGui::ShowDemoWindow();

      if (ImGui::Begin("control panel")) {
        ImGui::ColorEdit3("Background", &c.x);
        ImGui::BeginDisabled();
        ImGui::Checkbox("Arcball Mode", &activate_arcball);
        ImGui::Checkbox("Item hoverd", &item_hovered);
        ImGui::EndDisabled();
        ImGui::Checkbox("draw cube", &draw_cube);
        ImGui::Checkbox("draw plane", &draw_plane);
        if (ImGui::SliderInt(
                "MSAA samples", &msaa_samples_bits, 0, 3,
                fmt::format("{}", 1 << msaa_samples_bits).c_str())) {
          msaa_frame_buffer.SetSamples(1 << msaa_samples_bits);
        }
        ImGui::Text("Camera distance: %.2f", camera.GetCameraDistance());
        ImGui::Separator();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
      }
      ImGui::End();

      static ImGuiWindowFlags preview_window_flags =
          ImGuiWindowFlags_AlwaysAutoResize;
      if (ImGui::Begin("preview", nullptr, preview_window_flags)) {
        static ImTextureID texture_id;
        texture_id = reinterpret_cast<ImTextureID>(
            static_cast<intptr_t>(final_frame_buffer.ColorTexture()));

        window_pos = ImGui::GetCursorScreenPos();
        curr_pos = {lumos::Clamp(io.MousePos.x - window_pos.x, 0.0f,
                                 static_cast<float>(framebuffer_width)),
                    lumos::Clamp(io.MousePos.y - window_pos.y, 0.0f,
                                 static_cast<float>(framebuffer_height))};
        ImGui::Image(texture_id, {static_cast<float>(framebuffer_width),
                                  static_cast<float>(framebuffer_height)});
        // 有可能出现两个 window 重叠的情况，此时需要修改一下
        if (ImGui::IsItemHovered() && io.WantCaptureMouse) {
          // we have to disable window movement?
          preview_window_flags =
              ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
          item_hovered = true;
          if (ImGui::IsKeyDown(ImGuiKey_MouseLeft)) {
            if (!activate_arcball) {
              activate_arcball = true;
              prev_pos = curr_pos;
            }
          }
          if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft)) {
            activate_arcball = false;
          }
          camera.Zoom(io.MouseWheel);
        } else {
          preview_window_flags = ImGuiWindowFlags_AlwaysAutoResize;
          activate_arcball = false;
          item_hovered = false;
          prev_pos = {0.0f, 0.0f};
        }

        if (activate_arcball) {
          camera.Rotate(cursorPosToNDCCoord(prev_pos),
                        cursorPosToNDCCoord(curr_pos));
          prev_pos = curr_pos;
        }
      }
      ImGui::End();
    };

    instance.Loop(render);
  } catch (std::exception &e) {
    ERROR(e.what());
  }
}