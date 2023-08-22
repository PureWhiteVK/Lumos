#include "lumos/gui/framebuffer.h"

namespace lumos {
namespace gui {

void FrameBuffer::createDepthRenderBuffer() {
  glGenRenderbuffers(1, m_depth.Ptr());
  glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, m_depth_format, m_width, m_height);
  CheckOpenGLErrors();
}

void FrameBuffer::createFrameBuffer() {
  glGenFramebuffers(1, Base::Ptr());
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
  // create color texture
  m_color.Resize(m_width, m_height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_color, 0);
  if (m_depth_format != GL_NONE) {
    // create depth buffer
    createDepthRenderBuffer();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, m_depth);
  }

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw lumos::RuntimeError("Failed to create frame buffer");
  }
  CheckOpenGLErrors();
}

void FrameBuffer::Resize(int width, int height) {
  m_width = width;
  m_height = height;

  Base::Destroy();
  m_color.Destroy();
  m_depth.Destroy();

  createFrameBuffer();
}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) {
  if (this == &other) {
    return *this;
  }

  Base::operator=(std::move(other));
  m_color = std::move(other.m_color);
  m_depth = std::move(other.m_depth);

  m_width = other.m_width;
  m_height = other.m_height;
  m_depth_format = other.m_depth_format;

  other.m_width = 0;
  other.m_height = 0;
  other.m_depth_format = GL_NONE;
  return *this;
}

void MsaaFrameBuffer::createColorTexture() {
  glGenTextures(1, m_color.Ptr());
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_RGBA,
                          m_width, m_height, GL_TRUE);
  CheckOpenGLErrors();
}

void MsaaFrameBuffer::createDepthRenderBuffer() {
  glGenRenderbuffers(1, m_depth.Ptr());
  glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples, m_depth_format,
                                   m_width, m_height);
  CheckOpenGLErrors();
}

void MsaaFrameBuffer::createFrameBuffer() {
  glGenFramebuffers(1, Base::Ptr());
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
  createColorTexture();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D_MULTISAMPLE, m_color, 0);
  if (m_depth_format != GL_NONE) {
    createDepthRenderBuffer();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, m_depth);
  }
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    CheckOpenGLErrors();
    throw lumos::RuntimeError("Failed to create frame buffer");
  }
  CheckOpenGLErrors();
}

void MsaaFrameBuffer::Resize(int width, int height) {
  m_width = width;
  m_height = height;

  Base::Destroy();
  m_color.Destroy();
  m_depth.Destroy();

  createFrameBuffer();
}

void MsaaFrameBuffer::SetSamples(int msaa_samples) {
  DEBUG("MsaaFrameBUffer::SetSamples {}, previous samples: {}",msaa_samples, m_samples);
  if (msaa_samples > m_max_samples) {
    DEBUG("Set msaa_samples to {}", m_max_samples);
    msaa_samples = m_max_samples;
  }
  if (msaa_samples == m_samples) {
    return;
  }
  m_samples = msaa_samples;

  Base::Destroy();
  m_color.Destroy();
  m_depth.Destroy();

  createFrameBuffer();
}

MsaaFrameBuffer::MsaaFrameBuffer(int width, int height, GLenum depth_format,
                                 int msaa_samples)
    : m_width(width), m_height(height), m_depth_format(depth_format) {
  // GL_MAX_DEPTH_TEXTURE_SAMPLES
  // GL_MAX_COLOR_TEXTURE_SAMPLES
  // GL_MAX_SAMPLES
  GLint gl_max_depth_samples, gl_max_color_samples, gl_max_samples;
  glGetIntegerv(GL_MAX_SAMPLES, &gl_max_samples);
  glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &gl_max_depth_samples);
  glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &gl_max_color_samples);
  DEBUG("max samples: {}", gl_max_samples);
  DEBUG("max depth samples: {}", gl_max_depth_samples);
  DEBUG("max color samples: {}", gl_max_color_samples);
  m_max_samples = std::min(
      gl_max_samples, std::min(gl_max_color_samples, gl_max_depth_samples));
  DEBUG("set m_max_samples to {}", m_max_samples);
  m_samples = std::min(msaa_samples, m_max_samples);
  DEBUG("set m_samples to {}", m_samples);
  createFrameBuffer();
}

MsaaFrameBuffer &MsaaFrameBuffer::operator=(MsaaFrameBuffer &&other) {
  if (this == &other) {
    return *this;
  }

  Base::operator=(std::move(other));
  m_color = std::move(other.m_color);
  m_depth = std::move(other.m_depth);

  m_width = other.m_width;
  m_height = other.m_height;
  m_depth_format = other.m_depth_format;
  m_samples = other.m_samples;
  m_max_samples = other.m_max_samples;

  other.m_width = 0;
  other.m_height = 0;
  other.m_depth_format = GL_NONE;
  other.m_samples = 0;
  other.m_max_samples = 0;
  return *this;
}

} // namespace gui
} // namespace lumos