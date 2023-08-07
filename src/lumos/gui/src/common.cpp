#include "lumos/gui/common.h"
#include "lumos/core/exception.h"

#include <glad/gl.h>

namespace lumos {
namespace gui {
void checkOpenGLErrors() {
  const GLenum gl_status = glGetError();
  if (gl_status != GL_NO_ERROR) {
    throw RuntimeError("OpenGL error: {}", gl_status);
  }
}
} // namespace gui
} // namespace lumos