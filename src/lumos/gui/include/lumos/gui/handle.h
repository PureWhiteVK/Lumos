#pragma once

#include "lumos/gui/common.h"

#include <cstddef>
#include <utility>

namespace lumos {
namespace gui {

template <typename Traits> class ScopedHandle {
public:
  using Handle = typename Traits::Handle;

  ScopedHandle() : m_handle(Traits::Null()) {}
  ScopedHandle(Handle h) : m_handle(h) {}
  ScopedHandle(const ScopedHandle &) = delete;
  ScopedHandle(ScopedHandle &&other) { *this = other; }

  ScopedHandle &operator=(const ScopedHandle &) = delete;
  ScopedHandle &operator=(Handle h) {
    *this = ScopedHandle(h);
    return *this;
  }
  ScopedHandle &operator=(ScopedHandle &&other) {
    if (this == &other) {
      return *this;
    }

    Traits::Destroy(m_handle);
    m_handle = other.m_handle;
    other.m_handle = Traits::Null();
    return *this;
  }

  explicit operator bool() const noexcept { return Traits::IsValid(m_handle); }

  operator Handle() const noexcept { return m_handle; }

  Handle *Ptr() noexcept { return &m_handle; }

  ScopedHandle *operator->() noexcept { return this; }

  friend bool operator==(ScopedHandle lhs, ScopedHandle rhs) {
    return lhs.m_handle == rhs.m_handle;
  }

  friend bool operator!=(ScopedHandle lhs, ScopedHandle rhs) {
    return !(lhs == rhs);
  }

  void Destroy() {
    Traits::Destroy(m_handle);
    m_handle = Traits::Null();
  }

  ~ScopedHandle() { Traits::Destroy(m_handle); }

protected:
  Handle m_handle{};
};

struct ObjectHandleTraits {
  using Handle = GLuint;
  static bool IsValid(Handle h) noexcept { return static_cast<bool>(h); }
  static Handle Null() noexcept { return 0; }
};

struct ProgramHandleTraits : ObjectHandleTraits {
  static Handle Create() { return glCreateProgram(); }
  static void Destroy(Handle h) { glDeleteProgram(h); }
};

// TODO: 对于 shader 的 ShaderHandle 可能需要特化一下？
struct ShaderHandleTraits : ObjectHandleTraits {
  static Handle Create(GLenum type) { return glCreateShader(type); }
  static void Destroy(Handle h) { glDeleteShader(h); }
};

struct VertexArrayHandleTraits : ObjectHandleTraits {
  static Handle Create() {
    Handle h;
    glGenVertexArrays(1, &h);
    return h;
  }
  static void Destroy(Handle h) { glDeleteVertexArrays(1, &h); }
};

struct BufferHandleTraits : ObjectHandleTraits {
  static Handle Create() {
    Handle h;
    glGenBuffers(1, &h);
    return h;
  }
  static void Destroy(Handle h) { glDeleteBuffers(1, &h); }
};

struct FrameBufferHandleTraits : ObjectHandleTraits {
  static Handle Create() {
    Handle h;
    glGenFramebuffers(1, &h);
    return h;
  }
  static void Destroy(Handle h) { glDeleteFramebuffers(1, &h); }
};

struct RenderBufferHandleTraits : ObjectHandleTraits {
  static Handle Create() {
    Handle h;
    glGenRenderbuffers(1, &h);
    return h;
  }
  static void Destroy(Handle h) { glDeleteRenderbuffers(1, &h); }
};

struct TextureHandleTraits : ObjectHandleTraits {
  static Handle Create() {
    Handle h;
    glGenTextures(1, &h);
    return h;
  }
  static void Destroy(Handle h) { glDeleteTextures(1, &h); }
};

using ShaderHandle = ScopedHandle<ShaderHandleTraits>;
using ProgramHandle = ScopedHandle<ProgramHandleTraits>;
using VertexArrayHandle = ScopedHandle<VertexArrayHandleTraits>;
using BufferHandle = ScopedHandle<BufferHandleTraits>;
using FrameBufferHandle = ScopedHandle<FrameBufferHandleTraits>;
using RenderBufferHandle = ScopedHandle<RenderBufferHandleTraits>;
using TextureHandle = ScopedHandle<TextureHandleTraits>;
} // namespace gui
} // namespace lumos
