#pragma once

#include "lumos/gui/common.h"
#include "lumos/core/vector.h"

namespace lumos {
namespace gui {
// A simple wrapper for OpenGL textures.
// Textures are not resizeable, but can have their contents updated any number
// of times.
//
class Texture {
public:
  Texture() = default;
  Texture(int width, int height);
  // Updates the image stored in the texture with data.
  // The data is expected to be formatted as Width * Height pixels formatted
  // as BGRA pixels represented by uint8_t's.
  //
  // The data you pass to it must be of the same size as the dimensions used
  // to construct the texture.
  //
  void update(const ImageData4u8 &data, Vector2i offset = {0,0});

  void resize(int width, int height);

  // Get the OpenGL texture name.
  // OpenGL texture names are just uints that OpenGL uses as opaque pointers.
  //
  inline operator GLuint() const { return m_name; }

  // Gets the width of the texture.
  //
  inline int width() const { return m_width; }

  // Gets the height of the texture.
  //
  inline int height() const { return m_height; }

  ~Texture();

  // In the interest of simplicity, we don't refcount textures, so
  // copying doesn't work.
  //
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

  // Textures are moveable.
  //
  Texture(Texture &&other);
  Texture &operator=(Texture &&other);

private:
  // Deletes the wrapped texture.
  //
  void deleteTexture();

  void createTexture();

  static constexpr GLuint InvalidTextureName = 0;

  GLuint m_name = InvalidTextureName;
  int m_width = 0;
  int m_height = 0;
};
} // namespace gui
} // namespace lumos