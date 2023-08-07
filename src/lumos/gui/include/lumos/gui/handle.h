#pragma once

#include "lumos/core/common.h"

#include <cstddef>
#include <memory>

#include <glad/gl.h>

namespace lumos {
namespace gui {

template <typename Traits> class HandleWrapper {
public:
  using Handle = typename Traits::Handle;
  HandleWrapper() : handle(Traits::null()) {}
  HandleWrapper(std::nullptr_t) : handle(Traits::null()){};
  HandleWrapper(Handle h) : handle(h) {}

  explicit operator bool() const noexcept {
    return Traits::isValid(handle);
  }

  operator Handle() const noexcept { return handle; }

  Handle *ptr() noexcept { return &handle; }

  HandleWrapper* operator->() noexcept { return this; }

  friend bool operator==(HandleWrapper lhs, HandleWrapper rhs) {
    return lhs.handle == rhs.handle;
  }

  friend bool operator!=(HandleWrapper lhs, HandleWrapper rhs) {
    return !(lhs == rhs);
  }

private:
  Handle handle{};
};

} // namespace gui
} // namespace lumos
