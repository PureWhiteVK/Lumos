#include "lumos/gui/raii.h"
#include "lumos/core/common.h"

namespace lumos {
namespace gui {

void ProgramHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL program [{:03d}]", h);
  glDeleteProgram(h);
}

void ShaderHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL shader [{:03d}]", h);
  glDeleteShader(h);
}

void VertexArrayHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL VertexArray [{:03d}]", h);
  glDeleteVertexArrays(1, &h);
}

void BufferHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL Buffer [{:03d}]", h);
  glDeleteBuffers(1, &h);
}

void FrameBufferHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL FrameBuffer [{:03d}]", h);
  glDeleteFramebuffers(1, &h);
}

void RenderBufferHandleTraits::Destroy(Handle h) {
  DEBUG("delete OpenGL RenderBuffer [{:03d}]", h);
  glDeleteRenderbuffers(1, &h);
}

} // namespace gui
} // namespace lumos