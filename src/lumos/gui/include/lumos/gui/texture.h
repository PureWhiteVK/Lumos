#pragma once

#include "lumos/core/common.h"
#include "lumos/core/vector.h"
#include "lumos/gui/common.h"
#include "lumos/gui/handle.h"

namespace lumos {
namespace gui {
class RgbaTexture : public TextureHandle {
  using Base = TextureHandle;
public:
  RgbaTexture() = default;
  RgbaTexture(int width, int height);
  void Update(const ImageData4u8 &data, Vector2i offset = {0, 0});

  void Resize(int width, int height);

  int Width() const { return m_width; }

  int Height() const { return m_height; }

  RgbaTexture(RgbaTexture &&other);
  RgbaTexture &operator=(RgbaTexture &&other);

private:
  void createTexture();

  int m_width{};
  int m_height{};
};
} // namespace gui
} // namespace lumos