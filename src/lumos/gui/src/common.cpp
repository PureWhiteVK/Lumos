#include "lumos/gui/common.h"
#include "lumos/core/exception.h"
#include <exception>

#define NAME_PAIR(x)                                                           \
  { x, #x }

namespace lumos {
namespace gui {
const std::string &GetErrorTypeName(GLenum error_type) {

  static std::unordered_map<GLint, std::string> map{
      NAME_PAIR(GL_INVALID_ENUM),
      NAME_PAIR(GL_INVALID_VALUE),
      NAME_PAIR(GL_INVALID_OPERATION),
      NAME_PAIR(GL_INVALID_FRAMEBUFFER_OPERATION),
      NAME_PAIR(GL_OUT_OF_MEMORY),
#ifdef GL_STACK_UNDERFLOW
      NAME_PAIR(GL_STACK_UNDERFLOW),
#endif
#ifdef GL_STACK_OVERFLOW
      NAME_PAIR(GL_STACK_OVERFLOW),
#endif
  };
  if (map.find(error_type) == map.end()) {
    throw RuntimeError("Unknown OpenGL Error type: {}", error_type);
  }
  return map[error_type];
}

static const std::string &getShaderTypeName(GLint shader_type) {
  static std::unordered_map<GLint, std::string> map{
      NAME_PAIR(GL_VERTEX_SHADER),
      NAME_PAIR(GL_FRAGMENT_SHADER),
      NAME_PAIR(GL_GEOMETRY_SHADER),
  };
  if (map.find(shader_type) == map.end()) {
    throw RuntimeError("Unknown OpenGL Shader type: {}", shader_type);
  }
  return map[shader_type];
}

static std::string info_log{};

void CheckOpenGLShaderErrors(GLuint shader) {
  GLint shader_type;
  GLint info_log_length{};
  GLint success{};

  // Get shader info log from opengl
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success) {
    return;
  }
  CheckOpenGLErrors();
  glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > info_log.length()) {
    info_log.reserve(info_log_length);
  }
  info_log.resize(info_log_length);
  glGetShaderInfoLog(shader, info_log_length, nullptr, info_log.data());
  throw RuntimeError("OpenGL Shader [type={}] error:\n{}",
                     getShaderTypeName(shader_type), info_log);
}

void CheckOpenGLProgramErrors(GLuint program) {
  GLint info_log_length{};
  GLint link_status{};
  // Get shader info log from opengl
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status == GL_TRUE) {
    return;
  }
  CheckOpenGLErrors();
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > info_log.length()) {
    info_log.reserve(info_log_length);
  }
  info_log.resize(info_log_length);
  glGetProgramInfoLog(program, info_log_length, nullptr, info_log.data());
  throw RuntimeError("OpenGL Program error:\n{}", info_log);
}

template <>
void SetUniform(GLuint program, std::string name, const Eigen::Matrix4f &data) {
  GLint location = glGetUniformLocation(program, name.c_str());
  glUniformMatrix4fv(location, 1, data.IsRowMajor ? GL_FALSE : GL_TRUE,
                     data.data());
  CheckOpenGLErrors();
}

} // namespace gui
} // namespace lumos