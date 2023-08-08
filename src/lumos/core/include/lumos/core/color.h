#pragma once

#include "lumos/core/common.h"

namespace lumos {
template <typename Scalar, int Size, typename>
class Color : public Eigen::Vector<Scalar, Size> {
public:
  using Base = Eigen::Vector<Scalar, Size>;
  Color<Scalar, Size>(void) : Base() {}

  Color<Scalar, Size>(Scalar r, Scalar g, Scalar b)
      : Base(r, g, b, static_cast<Scalar>(1)) {}

  Color<Scalar, Size>(Scalar r, Scalar g, Scalar b, Scalar a)
      : Base(r, g, b, a) {}

  template <typename OtherDerived>
  Color<Scalar, Size>(const Eigen::MatrixBase<OtherDerived> &other)
      : Base(other) {}

  template <typename OtherDerived>
  Color<Scalar, Size> &operator=(const Eigen::MatrixBase<OtherDerived> &other) {
    this->Base::operator=(other);
    return *this;
  }

  Scalar &r() { return this->Base::x(); }
  Scalar &g() { return this->Base::y(); }
  Scalar &b() { return this->Base::z(); }
  Scalar &a() { return this->Base::w(); }

  const Scalar &r() const { return this->Base::x(); }
  const Scalar &g() const { return this->Base::y(); }
  const Scalar &b() const { return this->Base::z(); }
  const Scalar &a() const { return this->Base::w(); }
};
template <int Size> Color<float, Size> ToSrgb(const Color<float, Size> &c);
template <int Size> Color<float, Size> ToLinearRgb(const Color<float, Size> &c);

template <typename Scalar>
inline Color<Scalar, 3> ToColor3(const Color<Scalar, 4> &c) {
  return {c.r(), c.g(), c.b()};
}

template <typename Scalar>
inline Color<Scalar, 4> ToColor4(const Color<Scalar, 3> &c) {
  if constexpr (std::is_same_v<Scalar, uint8_t>) {
    return {c.r(), c.g(), c.b(), static_cast<Scalar>(255)};
  } else {
    return {c.r(), c.g(), c.b(), static_cast<Scalar>(1)};
  }
}

template <int Size>
inline Color<uint8_t, Size> ToUint8(const Color<float, Size> &c) {
  if constexpr (Size == 3) {
    return {
        // [0,1] ~ [0,255]
        static_cast<uint8_t>(Clamp(c.r(), 0.0f, 1.0f) * 255.0f),
        static_cast<uint8_t>(Clamp(c.g(), 0.0f, 1.0f) * 255.0f),
        static_cast<uint8_t>(Clamp(c.b(), 0.0f, 1.0f) * 255.0f),
    };
  } else {
    return {
        // [0,1] ~ [0,255]
        static_cast<uint8_t>(Clamp(c.r(), 0.0f, 1.0f) * 255.0f),
        static_cast<uint8_t>(Clamp(c.g(), 0.0f, 1.0f) * 255.0f),
        static_cast<uint8_t>(Clamp(c.b(), 0.0f, 1.0f) * 255.0f),
        static_cast<uint8_t>(Clamp(c.a(), 0.0f, 1.0f) * 255.0f),
    };
  }
}

template <int Size>
inline Color<float, Size> ToFloat(const Color<uint8_t, Size> &c) {
  if constexpr (Size == 3) {
    return {
        static_cast<float>(c.r()) / 255.0f,
        static_cast<float>(c.g()) / 255.0f,
        static_cast<float>(c.b()) / 255.0f,
    };
  } else {
    return {
        static_cast<float>(c.r()) / 255.0f,
        static_cast<float>(c.g()) / 255.0f,
        static_cast<float>(c.b()) / 255.0f,
        static_cast<float>(c.a()) / 255.0f,
    };
  }
}

inline Imf::Rgba ToImfRgba(const Color4f &c) {
  return {c.r(), c.g(), c.b(), c.a()};
}

inline Color4f ToColor4f(const Imf::Rgba &c) { return {c.r, c.g, c.b, c.a}; }
} // namespace lumos