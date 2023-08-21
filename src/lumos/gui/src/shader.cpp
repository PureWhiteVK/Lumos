#include "lumos/gui/shader.h"
#include "lumos/core/exception.h"

#include <fstream>
#include <sstream>

namespace lumos {
namespace gui {
void DrawProgram::loadShaderFile(const std::filesystem::path &path,
                                 std::string &output) {
  try {
    std::ifstream f(path);
    // when stream is bad or fail, throw exceptions!
    f.exceptions(std::ios::badbit | std::ios::failbit);
    std::stringstream s;
    s << f.rdbuf();
    f.close();
    output = s.str();
  } catch (...) {
     std::throw_with_nested( lumos::RuntimeError("failed to load shader file: {}",path.u8string()) );
  }
}

void DrawProgram::createProgram(const std::string &vertex_shader_source,
                                const std::string &fragment_shader_source) {
  m_handle = glCreateProgram();
  const char *const vertex_shader_source_ptr = vertex_shader_source.c_str();
  const char *const fragment_shader_source_ptr = fragment_shader_source.c_str();
  gui::ShaderHandle vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source_ptr, nullptr);
  glCompileShader(vertex_shader);
  gui::CheckOpenGLShaderErrors(vertex_shader);
  gui::ShaderHandle fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source_ptr, nullptr);
  glCompileShader(fragment_shader);
  gui::CheckOpenGLShaderErrors(fragment_shader);
  glAttachShader(m_handle, vertex_shader);
  glAttachShader(m_handle, fragment_shader);
  glLinkProgram(m_handle);
  gui::CheckOpenGLProgramErrors(m_handle);
  // get shader uniform info
  GLint num_values;
  GLsizei buffer_size;
  std::vector<char> name_buffer;
  GLint actual_length;

  glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, &num_values);
  DEBUG("#active uniforms: {}", num_values);
  m_active_uniform_info.reserve(num_values);
  glGetProgramiv(m_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffer_size);
  name_buffer.resize(buffer_size);
  // TODO: we can save uniform data to perform sanity check here!
  for (int i = 0; i < num_values; i++) {
    ActiveValueInfo value_info;
    glGetActiveUniform(m_handle, i, buffer_size, &actual_length,
                       &value_info.size, &value_info.type, name_buffer.data());
    value_info.name =
        std::string(name_buffer.data(), name_buffer.data() + actual_length);
    DEBUG("active uniform [{}] name: {}, size: {}, type: {}", i,
          value_info.name, value_info.size, gui::GetTypeName(value_info.type));
    m_active_uniform_info.emplace_back(value_info);
  }

  glGetProgramiv(m_handle, GL_ACTIVE_ATTRIBUTES, &num_values);
  DEBUG("#active attributes: {}", num_values);
  m_active_attribute_info.reserve(num_values);
  glGetProgramiv(m_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &buffer_size);
  name_buffer.resize(buffer_size);
  // TODO: we can save uniform data to perform sanity check here!
  for (int i = 0; i < num_values; i++) {
    ActiveValueInfo value_info;
    glGetActiveAttrib(m_handle, i, buffer_size, &actual_length,
                      &value_info.size, &value_info.type, name_buffer.data());
    name_buffer.resize(actual_length);
    value_info.name =
        std::string(name_buffer.data(), name_buffer.data() + actual_length);
    DEBUG("active attribute [{}] name: {}, size: {}, type: {}", i,
          value_info.name, value_info.size, gui::GetTypeName(value_info.type));
    m_active_attribute_info.emplace_back(value_info);
  }
}

void DrawProgram::PrintActiveAttributes() {
  DEBUG("#active attributes: {}", m_active_attribute_info.size());
  int i = 0;
  for (const auto &value_info : m_active_attribute_info) {
    DEBUG("active attribute [{}] name: {}, size: {}, type: {}", i,
          value_info.name, value_info.size, gui::GetTypeName(value_info.type));
    i++;
  }
}

void DrawProgram::PrintActiveUniforms() {
  DEBUG("#active uniforms: {}", m_active_uniform_info.size());
  int i = 0;
  for (const auto &value_info : m_active_attribute_info) {
    DEBUG("active attribute [{}] name: {}, size: {}, type: {}", i,
          value_info.name, value_info.size, gui::GetTypeName(value_info.type));
    i++;
  }
}

} // namespace gui
} // namespace lumos