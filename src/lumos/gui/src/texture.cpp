#include "lumos/gui/texture.h"
#include "lumos/core/color.h"
#include "lumos/core/common.h"

namespace lumos {
namespace gui {

RgbaTexture::RgbaTexture(int width, int height)
    : m_width(width), m_height(height) {
  createTexture();
}

RgbaTexture::RgbaTexture(RgbaTexture &&other) {
  *this = std::move(other);
}

RgbaTexture &RgbaTexture::operator=(RgbaTexture &&other) {
  if (this == &other) {
    return *this;
  }

  Base::operator=(std::move(other));

  m_height = other.m_height;
  m_width = other.m_width;

  other.m_height = 0;
  other.m_width = 0;

  return *this;
}

void RgbaTexture::createTexture() {
  glGenTextures(1, &m_handle);
  DEBUG("Create OpenGL rgba texture [{:#03x}], wxh: {}x{}", m_handle, m_width,
        m_height);
  glBindTexture(GL_TEXTURE_2D, m_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  CheckOpenGLErrors();
}

void RgbaTexture::Resize(int width, int height) {
  if (width != m_width || height != m_height) {
    Base::Destroy();
    m_width = width;
    m_height = height;
    createTexture();
  }
}

void RgbaTexture::Update(const ImageData4u8 &data, Vector2i offset) {
  DEBUG("Update OpenGL rgba texture [{:#03x}]", m_handle);
  glBindTexture(GL_TEXTURE_2D, m_handle);
  glTexSubImage2D(GL_TEXTURE_2D,    // target
                  0,                // level
                  offset.x(),       // xoffset
                  offset.y(),       // yoffset
                  data.cols(),      // width
                  data.rows(),      // height
                  GL_RGBA,          // format
                  GL_UNSIGNED_BYTE, // type
                  data.data());     // pixels
  CheckOpenGLErrors();
}
} // namespace gui
} // namespace lumos