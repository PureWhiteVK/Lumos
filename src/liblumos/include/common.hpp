#pragma once

// #define LOG(LOG_TYPE, ...)                                                     \
//   do {                                                                         \
//     spdlog::LOG_TYPE(__VA_ARGS__);                                             \
//   } while (false)

// #define TRACE(...) LOG(trace, __VA_ARGS__)
// #define DEBUG(...) LOG(debug, __VA_ARGS__)
// #define INFO(...) LOG(info, __VA_ARGS__)
// #define WARN(...) LOG(warn, __VA_ARGS__)
// #define ERROR(...) LOG(error, __VA_ARGS__)
// #define CRITICAL(...) LOG(error, __VA_ARGS__)

namespace lumos {
inline void CheckOpenGLErrors() {
  const GLenum glStatus = glGetError();
  if (glStatus != GL_NO_ERROR) {
    throw std::runtime_error(fmt::format("OpenGL error: {}",static_cast<int>(glStatus)));
  }
}
} // namespace lumos