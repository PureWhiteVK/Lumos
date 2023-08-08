
#include "lumos/core/common.h"
#include "lumos/core/exception.h"
#include "lumos/gui/common.h"
#include "lumos/gui/context.h"
#include "lumos/gui/raii.h"
#include "lumos/gui/texture.h"

#include <exception>

#include <Eigen/Dense>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
// #include <Eigen/Geometry>

/*
TODO:
2. get rid of glm (switching to eigen!)
# 需要将纯相机类和相机交互类分离开？，gui部分实现交互，然后 core 部分就是纯相机，包含 view 和 projection matrix？
3. camera class implementation (offline and online?)
4. 对于需要 bind 的东西，例如 shader 和 vao 等，添加一个
scope_binder？（RAII，离开 scope 的时候自动清除）
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
    // try load test shader
    static const char *vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoord;
uniform mat4 mvp;
out vec3 normal;
void main() {
  gl_Position = mvp * vec4(position, 1.0);
  // normal = vec3(0.5,0.5,0.5);
  normal = vertexNormal;
}
)";

    static const char *fragment_shader_source = R"(
#version 330 core
in vec3 normal;
out vec4 fragColor;
vec4 normal_fragment_shader(vec3 n){
  return vec4((n + vec3(1.0)) / 2.0,1.0);
}
void main() {
  fragColor = normal_fragment_shader(normal);
}
)";

    IndexBuffer cube_indices{
        {0, 1, 2},    {2, 3, 0},    // v0-v1-v2, v2-v3-v0 (front)
        {4, 5, 6},    {6, 7, 4},    // v0-v3-v4, v4-v5-v0 (right)
        {8, 9, 10},   {10, 11, 8},  // v0-v5-v6, v6-v1-v0 (top)
        {12, 13, 14}, {14, 15, 12}, // v1-v6-v7, v7-v2-v1 (left)
        {16, 17, 18}, {18, 19, 16}, // v7-v4-v3, v3-v2-v7 (bottom)
        {20, 21, 22}, {22, 23, 20}, // v4-v7-v6, v6-v5-v4 (back)
    };
    DEBUG("cube_indices.shape: {},{}", cube_indices.rows(),
          cube_indices.cols());

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

    DEBUG("cube_vertices.shape: {},{}", cube_vertices.rows(),
          cube_vertices.cols());

    gui::ProgramHandle shader_program = glCreateProgram();
    {
      gui::ShaderHandle vertex_shader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
      glCompileShader(vertex_shader);
      gui::CheckOpenGLShaderErrors(vertex_shader);
      gui::ShaderHandle fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
      glCompileShader(fragment_shader);
      gui::CheckOpenGLShaderErrors(fragment_shader);
      glAttachShader(shader_program, vertex_shader);
      glAttachShader(shader_program, fragment_shader);
      glLinkProgram(shader_program);
      gui::CheckOpenGLProgramErrors(shader_program);
    }

    gui::VertexArrayHandle VAO;
    gui::BufferHandle VBO, EBO;
    {
      glGenVertexArrays(1, VAO.Ptr());
      glGenBuffers(1, VBO.Ptr());
      glGenBuffers(1, EBO.Ptr());

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(decltype(cube_vertices)::Scalar) *
                       cube_vertices.size(),
                   cube_vertices.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(decltype(cube_indices)::Scalar) * cube_indices.size(),
                   cube_indices.data(), GL_STATIC_DRAW);

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

    int framebuffer_width = 800;
    int framebuffer_height = 600;
    // create rgb texture here
    gui::Texture framebuffer_texture(framebuffer_width, framebuffer_height);
    gui::RenderBufferHandle depth_rb;
    gui::FrameBufferHandle FBO;
    {
      glGenFramebuffers(1, FBO.Ptr());
      glBindFramebuffer(GL_FRAMEBUFFER, FBO);
      // Attach 2D texture to this FBO
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, framebuffer_texture, 0);
      //-------------------------
      DEBUG("bind depth texture");
      glGenRenderbuffers(1, depth_rb.Ptr());
      glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                            framebuffer_width, framebuffer_height);
      //-------------------------
      // Attach depth buffer to FBO
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, depth_rb);
      // sanity check
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw lumos::RuntimeError("failed to setup frame buffer");
      }
      CheckOpenGLErrors();
    }

    // view
    ArcballCamera camera =
        ArcballCamera(glm::vec3{0.0f, 0.0f, 5.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

    // projection
    glm::mat4 projection =
        glm::perspective(glm::radians<float>(45.0f),
                         static_cast<float>(framebuffer_width) /
                             static_cast<float>(framebuffer_height),
                         0.01f, 100.0f);
    // model will change with time?
    const ImGuiIO &io = ImGui::GetIO();
    ImVec4 c{0.3f, 0.2f, 0.1f, 1.0f};

    glm::vec3 center{0.0f, 0.0f, 0.0f};

    const unsigned int CENTER_X = instance.width() / 2;
    const unsigned int CENTER_Y = instance.height() / 2;

    ImVec2 prev_pos;

    auto cursorPosToNDCCoord = [=](const ImVec2 &pos) -> glm::vec2 {
      // 此处输入的坐标是xPos,yPos，有可能为两个负值
      // 在OpenGL中，x轴朝右，y轴朝下，而NDC中y轴朝上，需要进行翻转，同时将其中心移动到中点
      // 需要将鼠标位置映射到[-1,1]区间
      return {lumos::Clamp((pos.x - CENTER_X) / CENTER_X, -1.0f, 1.0f),
              lumos::Clamp((CENTER_Y - pos.y) / CENTER_Y, -1.0f, 1.0f)};
    };

    bool activate_arcball = false;

    auto render = [&]() {
      // IO
      if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        glfwSetWindowShouldClose(instance, true);
      }
      if (ImGui::IsKeyDown(ImGuiKey_MouseLeft)) {
        if (!activate_arcball) {
          activate_arcball = true;
          prev_pos = io.MousePos;
        }
      }
      if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft)) {
        activate_arcball = false;
      }

      // RENDER TO TEXTURE
      {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(c.x, c.y, c.z, c.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        // update camera
        if (activate_arcball) {
          camera.Rotate(cursorPosToNDCCoord(prev_pos),
                        cursorPosToNDCCoord(io.MousePos));
          prev_pos = io.MousePos;
        }
        glUseProgram(shader_program);
        glm::mat4 model{1.0f};
        gui::SetUniform(shader_program, "mvp",
                        projection * camera.GetViewMatrix() * model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, cube_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
      }

      // IMGUI
      ImGui::ShowDemoWindow();

      if (ImGui::Begin("control panel")) {
        ImGui::ColorEdit3("Background", &c.x);
        ImGui::BeginDisabled();
        ImGui::Checkbox("Arcball Mode", &activate_arcball);
        ImGui::EndDisabled();
        ImGui::Text("Camera distance: %.2f", camera.GetCameraDistance());
        ImGui::Text("prev pos: (%.2f,%.2f)", prev_pos.x, prev_pos.y);
        ImGui::Text("curr pos: (%.2f,%.2f)", io.MousePos.x, io.MousePos.y);
        auto ndc_prev = cursorPosToNDCCoord(prev_pos);
        auto ndc_curr = cursorPosToNDCCoord(io.MousePos);
        ImGui::Text("ndc prev pos: (%.2f,%.2f)", ndc_prev.x, ndc_prev.y);
        ImGui::Text("ndc curr pos: (%.2f,%.2f)", ndc_curr.x, ndc_curr.y);
        ImGui::Separator();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
      }
      ImGui::End();

      if (ImGui::Begin("preview", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // static ImVec2 window_size, window_padding, image_size, pos,
        //     relative_pos;
        static ImTextureID texture_id;
        // window_size = ImGui::GetWindowSize();
        // window_padding = ImGui::GetStyle().WindowPadding;
        // image_size.x =
        //     std::min(window_size.x - 2.0f * window_padding.x,
        //              static_cast<float>(framebuffer_texture.width()));

        // image_size.y = static_cast<float>(framebuffer_texture.height()) /
        //                static_cast<float>(framebuffer_texture.width()) *
        //                image_size.x;
        texture_id = reinterpret_cast<ImTextureID>(
            static_cast<intptr_t>(framebuffer_texture));
        ImGui::Image(texture_id,
                     {static_cast<float>(framebuffer_texture.width()),
                      static_cast<float>(framebuffer_texture.height())});
      }
      ImGui::End();
    };

    instance.Loop(render);
  } catch (std::exception &e) {
    ERROR(e.what());
  }
}