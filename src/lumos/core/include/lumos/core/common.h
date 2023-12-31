#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>

#include <Eigen/Core>
#include <ImfRgba.h>
#include <spdlog/spdlog.h>

#define LOG(LOG_TYPE, ...)                                                     \
  do {                                                                         \
    spdlog::LOG_TYPE(__VA_ARGS__);                                             \
  } while (false)

#define DEBUG(...) LOG(debug, __VA_ARGS__)
#define INFO(...) LOG(info, __VA_ARGS__)
#define WARN(...) LOG(warn, __VA_ARGS__)
#define ERROR(...) LOG(error, __VA_ARGS__)
#define FATAL(...) LOG(fatal, __VA_ARGS__)

constexpr float Epsilon = 1e-4f;
constexpr float PI = 3.14159265358979323846f;
constexpr float INV_PI = 0.31830988618379067154f;
constexpr float INV_TWOPI = 0.15915494309189533577f;
constexpr float INV_FOURPI = 0.07957747154594766788f;
constexpr float SQRT_TWO = 1.41421356237309504880f;
constexpr float INV_SQRT_TWO = 0.70710678118654752440f;

namespace lumos {
static constexpr const char *LOGGER_NAME = "lumos";
class RuntimeError;

template <typename Scalar, int Size>
class Vector;
using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;
using Vector2u = Vector<uint32_t, 2>;
using Vector3u = Vector<uint32_t, 3>;
using Vector4u = Vector<uint32_t, 4>;
using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;
using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;

template <typename Scalar, int Size>
class Color;
using Color3f = Color<float, 3>;
using Color4f = Color<float, 4>;
using Color3u8 = Color<uint8_t, 3>;
using Color4u8 = Color<uint8_t, 4>;

// 这个只能算 image_data
template <typename Pixel>
using ImageData =
    Eigen::Array<Pixel, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using ImageData4h = ImageData<Imf::Rgba>;
using ImageData3u8 = ImageData<Color3u8>;
using ImageData4u8 = ImageData<Color4u8>;
using ImageData3f = ImageData<Color3f>;
using ImageData4f = ImageData<Color4f>;

template <typename Pixel> using ImageDataView = Eigen::Map<ImageData<Pixel>>;
using ImageDataView4h = ImageDataView<Imf::Rgba>;
using ImageDataView3u8 = ImageDataView<Color3u8>;
using ImageDataView4u8 = ImageDataView<Color4u8>;
using ImageDataView3f = ImageDataView<Color3f>;
using ImageDataView4f = ImageDataView<Color4f>;

inline float RadToDeg(float value) { return value * (180.0f / PI); }
inline float DegToRad(float value) { return value * (PI / 180.0f); }

template <typename T> inline T Clamp(T value, T min, T max) {
  return std::min(std::max(value, min), max);
}

std::shared_ptr<spdlog::logger>
SetupLogger(const std::vector<spdlog::sink_ptr> &sinks);
std::filesystem::path GetDataPath(const std::filesystem::path& path);
std::filesystem::path GetDataPathUtf8(std::string_view name);
}; // namespace lumos