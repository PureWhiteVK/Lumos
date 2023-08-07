#pragma once

#include "common.h"
#include <stdexcept>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/ranges.h>


namespace lumos {
class RuntimeError : public std::runtime_error {
public:
  template <typename... T>
  RuntimeError(fmt::format_string<T...> fmt, T &&...args)
      : std::runtime_error(fmt::format(fmt, args...)) {}
};
} // namespace lumos