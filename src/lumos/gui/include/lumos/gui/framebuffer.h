#include "lumos/core/common.h"
#include "lumos/gui/common.h"
#include "lumos/gui/handle.h"
#include "lumos/gui/texture.h"

namespace lumos {
namespace gui {
class FrameBuffer : public FrameBufferHandle {
  using Base = FrameBufferHandle;

public:
  FrameBuffer(int width, int height, GLenum depth_format = GL_NONE)
      : m_width(width), m_height(height), m_depth_format(depth_format) {
    createFrameBuffer();
  }
  void Resize(int width, int height);
  int Width() const { return m_width; }
  int Height() const { return m_height; }
  RgbaTexture::Handle ColorTexture() const { return m_color; }

  FrameBuffer(FrameBuffer &&other) { *this = std::move(other); }
  FrameBuffer &operator=(FrameBuffer &&other);
  FrameBuffer(const FrameBuffer &) = delete;
  FrameBuffer &operator=(const FrameBuffer &) = delete;

private:
  void createFrameBuffer();
  void createDepthRenderBuffer();

private:
  RgbaTexture m_color{};
  // we can also use the texture as depth buffer (for shadow mapping!)
  RenderBufferHandle m_depth{};
  int m_width{};
  int m_height{};
  GLenum m_depth_format{};
};

class MsaaFrameBuffer : public FrameBufferHandle {
  using Base = FrameBufferHandle;

public:
  MsaaFrameBuffer(int width, int height,
                  GLenum depth_format = GL_DEPTH_COMPONENT24,
                  int msaa_samples = 4);

  void Resize(int width, int height);
  void SetSamples(int msaa_samples);
  int Width() const { return m_width; }
  int Height() const { return m_height; }

  MsaaFrameBuffer(MsaaFrameBuffer &&other) { *this = std::move(other); }
  MsaaFrameBuffer &operator=(MsaaFrameBuffer &&other);
  MsaaFrameBuffer(const MsaaFrameBuffer &) = delete;
  MsaaFrameBuffer &operator=(const MsaaFrameBuffer &) = delete;

private:
  void createColorTexture();
  void createDepthRenderBuffer();
  void createFrameBuffer();

private:
  TextureHandle m_color{};
  RenderBufferHandle m_depth{};
  int m_width{};
  int m_height{};
  GLenum m_depth_format{};
  int m_samples{};
  int m_max_samples{};
};
} // namespace gui
} // namespace lumos