#pragma once

#include "lumos/core/common.h"

namespace lumos {
template <typename Scalar, int Size, typename>
class Vector : public Eigen::Vector<Scalar, Size> {
public:
  using Base = Eigen::Vector<Scalar, Size>;
  using Derived = Vector<Scalar, Size>;
  Vector<Scalar, Size>(void) : Base() {}
  Vector<Scalar, Size>(Scalar x, Scalar y) : Base(x, y) {}
  Vector<Scalar, Size>(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}
  Vector<Scalar, Size>(Scalar x, Scalar y, Scalar z, Scalar w)
      : Base(x, y, z, w) {}

  template <typename OtherDerived>
  Vector<Scalar, Size>(const Eigen::MatrixBase<OtherDerived> &other)
      : Base(other) {}

  template <typename OtherDerived>
  Vector<Scalar, Size> &
  operator=(const Eigen::MatrixBase<OtherDerived> &other) {
    this->Base::operator=(other);
    return *this;
  }

  Scalar length() const {
    std::string s;
      s.clear();
    return sqrt(lengthSquared());
  }

  Scalar lengthSquared() const {
    const Derived& v = *this;
    return v * v;
  }
};
} // namespace lumos