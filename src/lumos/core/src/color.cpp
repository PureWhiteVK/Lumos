#include "lumos/core/color.h"

namespace lumos {
template <int Size> Color<float, Size> toSrgb(const Color<float, Size> &c) {
  Color<float, Size> result;
  for (int i = 0; i < 3; ++i) {
    float value = std::max(6.10352e-5f, c.coeff(i));
    result[i] = std::min(
        value * 12.92f,
        std::pow(std::max(value, 0.00313067f), 1.0f / 2.4f) * 1.055f - 0.055f);
  }
  if constexpr (Size == 4) {
    result[3] = c.a();
  }
  return result;
}

template <int Size>
Color<float, Size> toLinearRgb(const Color<float, Size> &c) {
  Color<float, Size> result;
  for (int i = 0; i < 3; ++i) {
    float value = std::max(6.10352e-5f, c.coeff(i));
    result[i] = value > 0.04045f
                    ? std::pow(value * (1.0f / 1.055f) + 0.0521327f, 2.4f)
                    : value * (1.0f / 12.92f);
  }
  if constexpr (Size == 4) {
    result[3] = c.a();
  }
  return result;
}

// 外部类使用的模板函数必须显式模版实例化
template Color3f toSrgb(const Color3f &);
template Color4f toSrgb(const Color4f &);
template Color3f toLinearRgb(const Color3f &);
template Color4f toLinearRgb(const Color4f &);
} // namespace lumos