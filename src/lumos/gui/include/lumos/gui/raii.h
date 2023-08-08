#pragma once

#include "lumos/gui/common.h"

#include <cstddef>
#include <utility>

namespace lumos {
namespace gui {

template <typename Traits> class HandleWrapper {
public:
  using Handle = typename Traits::Handle;

  HandleWrapper() : m_handle(Traits::Null()) {}
  HandleWrapper(Handle h) : m_handle(h) {}
  HandleWrapper(const HandleWrapper &) = delete;
  HandleWrapper(HandleWrapper &&other) { *this = other; }

  HandleWrapper &operator=(const HandleWrapper &) = delete;
  HandleWrapper &operator=(Handle h) {
    *this = HandleWrapper(h);
    return *this;
  }
  HandleWrapper &operator=(HandleWrapper &&other) {
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

  HandleWrapper *operator->() noexcept { return this; }

  friend bool operator==(HandleWrapper lhs, HandleWrapper rhs) {
    return lhs.m_handle == rhs.m_handle;
  }

  friend bool operator!=(HandleWrapper lhs, HandleWrapper rhs) {
    return !(lhs == rhs);
  }

  ~HandleWrapper() { Traits::Destroy(m_handle); }

private:
  Handle m_handle{};
};

struct ObjectHandleTraits {
  using Handle = GLuint;
  static bool IsValid(Handle h) noexcept { return static_cast<bool>(h); }
  static Handle Null() noexcept { return 0; }
};

struct ProgramHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

struct ShaderHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

struct VertexArrayHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

struct BufferHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

struct FrameBufferHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

struct RenderBufferHandleTraits : ObjectHandleTraits {
  static void Destroy(Handle h);
};

using ShaderHandle = HandleWrapper<ShaderHandleTraits>;
using ProgramHandle = HandleWrapper<ProgramHandleTraits>;
using VertexArrayHandle = HandleWrapper<VertexArrayHandleTraits>;
using BufferHandle = HandleWrapper<BufferHandleTraits>;
using FrameBufferHandle = HandleWrapper<FrameBufferHandleTraits>;
using RenderBufferHandle = HandleWrapper<RenderBufferHandleTraits>;
} // namespace gui
} // namespace lumos
