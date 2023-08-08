#include "lumos/gui/texture.h"
#include "lumos/core/color.h"
#include "lumos/core/common.h"

namespace lumos {
namespace gui {

Texture::Texture(int width, int height) : m_width(width), m_height(height) {
  createTexture();
}

Texture::Texture(Texture &&other) { *this = std::move(other); }

Texture &Texture::operator=(Texture &&other) {
  if (this == &other) {
    return *this;
  }

  deleteTexture();

  m_name = other.m_name;
  m_height = other.m_height;
  m_width = other.m_width;

  other.m_name = InvalidTextureName;
  other.m_height = 0;
  other.m_width = 0;

  return *this;
}

Texture::~Texture() { deleteTexture(); }

void Texture::deleteTexture() {
  if (m_name) {
    DEBUG("delete texture[{:#03x}]", m_name);
  }
  glDeleteTextures(1, &m_name);
}

void Texture::createTexture() {
  glGenTextures(1, &m_name);
  DEBUG("create texture[{:#03x}], wxh: {}x{}", m_name, m_width, m_height);
  glBindTexture(GL_TEXTURE_2D, m_name);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  CheckOpenGLErrors();
}

void Texture::resize(int width, int height) {
  if (width != m_width || height != m_height) {
    deleteTexture();
    m_width = width;
    m_height = height;
    createTexture();
  }
}

void Texture::update(const ImageData4u8 &data, Vector2i offset) {
  DEBUG("update texture[{:#03x}]", m_name);
  glBindTexture(GL_TEXTURE_2D, m_name);
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