#pragma once

#include "lumos/core/common.h"
#include "lumos/core/exception.h"

#include <glad/gl.h>

#include <string>

namespace lumos {
namespace gui {

const std::string &GetErrorTypeName(GLenum error_type);

#define CheckOpenGLErrors() \
  do { \
    const GLenum error_type = glGetError(); \
    if (error_type != GL_NO_ERROR) { \
      throw lumos::RuntimeError("OpenGL error: {} at file: {},line: {}", lumos::gui::GetErrorTypeName(error_type), __FILE__, __LINE__); \
    } \
  } while(false)

void CheckOpenGLShaderErrors(GLuint shader);
void CheckOpenGLProgramErrors(GLuint program);

template<typename T>
void SetUniform(GLuint program,std::string name, const T& data);

} // namespace gui
} // namespace lumos