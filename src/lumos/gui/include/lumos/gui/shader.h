#pragma once

#include "lumos/core/common.h"
#include "lumos/gui/common.h"
#include "lumos/gui/handle.h"

#include <filesystem>
#include <string>

namespace lumos {
namespace gui {

struct ActiveValueInfo {
  GLint size{};
  GLenum type{};
  std::string name{};
};

// only support vertex & fragment shader linkage
class DrawProgram : public ProgramHandle {
  using Base = ProgramHandle;

public:
  DrawProgram(const std::filesystem::path &vertex_shader_path,
              const std::filesystem::path &fragment_shader_path) {
    std::string vertex_shader_source;
    std::string fragment_shader_source;
    loadShaderFile(vertex_shader_path, vertex_shader_source);
    loadShaderFile(fragment_shader_path, fragment_shader_source);
    createProgram(vertex_shader_source, fragment_shader_source);
  }

  DrawProgram(const std::string &vertex_shader_source,
              const std::string &fragment_shader_source) {
    createProgram(vertex_shader_source, fragment_shader_source);
  }

  DrawProgram(const char *vertex_shader_source,
              const char *fragment_shader_source) {
    createProgram(vertex_shader_source, fragment_shader_source);
  }

  void PrintActiveUniforms();
  void PrintActiveAttributes();

private:
  void createProgram(const std::string &vertex_shader_source,
                     const std::string &fragment_shader_source);
  void loadShaderFile(const std::filesystem::path &path, std::string &output);

private:
  std::vector<ActiveValueInfo> m_active_uniform_info{};
  std::vector<ActiveValueInfo> m_active_attribute_info{};
};
} // namespace gui
} // namespace lumos