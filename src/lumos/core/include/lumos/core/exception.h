#include "common.h"
#include <stdexcept>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/ranges.h>


namespace lumos {
class runtime_error : public std::runtime_error {
public:
  template <typename... T>
  runtime_error(fmt::format_string<T...> fmt, T &&...args)
      : std::runtime_error(fmt::format(fmt, args...)) {}
};
} // namespace lumos