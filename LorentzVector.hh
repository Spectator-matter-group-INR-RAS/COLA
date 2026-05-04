/**
 * Copyright (c) 2024-2025 Alexandr Svetlichnyi, Savva Savenkov, Artemii Novikov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef COLA_LORENTZVECTOR_HH
#define COLA_LORENTZVECTOR_HH

#include <array>
#include <cmath>
#include <iostream>

namespace cola {

  template <typename Type = double> class Vector3 {
   public:
    Type x, y, z;

   private:
    using FieldPtr = Type Vector3::*;

    static constexpr std::array<FieldPtr, 3> kFields = {&Vector3::x, &Vector3::y, &Vector3::z};

   public:
    const Type& operator[](int i) const { return this->*kFields[i]; }
    Type& operator[](int i) { return this->*kFields[i]; }

    std::enable_if_t<std::is_arithmetic_v<Type>, Vector3&> operator+=(const Vector3& other) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] += other.*kFields[i];
      }
      return *this;
    }

    std::enable_if_t<std::is_arithmetic_v<Type>, Vector3&> operator-=(const Vector3& other) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] -= other.*kFields[i];
      }
      return *this;
    }

    std::enable_if_t<std::is_arithmetic_v<Type>, Vector3&> operator*=(Type scalar) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] *= scalar;
      }
      return *this;
    }

    std::enable_if_t<std::is_arithmetic_v<Type>, Vector3&> operator/=(Type scalar) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] /= scalar;
      }
      return *this;
    }

    std::enable_if_t<std::is_arithmetic_v<Type>, Type> Mag2() const { return x * x + y * y + z * z; }
    std::enable_if_t<std::is_arithmetic_v<Type>, Type> Mag() const { return std::sqrt(Mag2()); }
  };

  template <typename Type>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator+(const Vector3<Type>& a,
                                                                        const Vector3<Type>& b) {
    auto res = a;
    res += b;
    return res;
  }

  template <typename Type>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator-(const Vector3<Type>& a,
                                                                        const Vector3<Type>& b) {
    auto res = a;
    res -= b;
    return res;
  }

  template <typename Type>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator-(const Vector3<Type>& a) {
    auto res = a;
    res.x = -res.x;
    res.y = -res.y;
    res.z = -res.z;
    return res;
  }

  template <typename Type, typename Scalar>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator*(const Vector3<Type>& vec, Scalar scalar) {
    auto res = vec;
    res *= scalar;
    return res;
  }

  template <typename Type, typename Scalar>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator*(Scalar scalar, const Vector3<Type>& vec) {
    return vec * scalar;
  }

  template <typename Type, typename Scalar>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> operator/(const Vector3<Type>& vec, Scalar scalar) {
    auto res = vec;
    res /= scalar;
    return res;
  }

  template <typename Type> bool operator==(const Vector3<Type>& a, const Vector3<Type>& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }

  template <typename Type> bool operator!=(const Vector3<Type>& a, const Vector3<Type>& b) { return !(a == b); }

  template <typename Type> std::ostream& operator<<(std::ostream& out, const Vector3<Type>& vec) {
    out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out;
  }

  // assuming decart coordinate systems, get Vector coordinates in a new system, where uzVec is Oz unit vector
  // coordinates in the old system.
  template <typename Type>
  std::enable_if_t<std::is_arithmetic_v<Type>, Vector3<Type>> RotateUz(const Vector3<Type> stVec,
                                                                       const Vector3<Type> uzVec) {
    // NewUzVector must be normalized !

    Vector3<Type> res_vec;
    double up = uzVec.x * uzVec.x + uzVec.y * uzVec.y;

    if (up > 0) {
      up = std::sqrt(up);
      res_vec.x = (uzVec.x * uzVec.z * stVec.x - uzVec.y * stVec.y) / up + uzVec.x * stVec.z;
      res_vec.y = (uzVec.y * uzVec.z * stVec.x + uzVec.x * stVec.y) / up + uzVec.y * stVec.z;
      res_vec.z = -up * stVec.x + uzVec.z * stVec.z;
    } else if (uzVec.z < 0.) {
      res_vec.x = -stVec.x;
      res_vec.y = stVec.y;
      res_vec.z = -stVec.z;
    }  // phi=0  teta=pi

    return res_vec;
  }

  template <typename Type = double> class LorentzVectorImpl {
   public:
    union {
      Type e;
      Type t;
    };
    Type x;
    Type y;
    Type z;

   private:
    using FieldPtr = Type LorentzVectorImpl::*;

    static constexpr std::array<FieldPtr, 4> kFields = {&LorentzVectorImpl::e, &LorentzVectorImpl::x,
                                                        &LorentzVectorImpl::y, &LorentzVectorImpl::z};

   public:
    const Type& operator[](int i) const { return this->*kFields[i]; }
    Type& operator[](int i) { return this->*kFields[i]; }

    LorentzVectorImpl& operator+=(const LorentzVectorImpl& other) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] += other.*kFields[i];
      }
      return *this;
    }

    LorentzVectorImpl& operator-=(const LorentzVectorImpl& other) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] -= other.*kFields[i];
      }
      return *this;
    }

    LorentzVectorImpl& operator*=(Type scalar) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] *= scalar;
      }
      return *this;
    }

    LorentzVectorImpl& operator/=(Type scalar) {
      for (size_t i = 0; i < kFields.size(); ++i) {
        this->*kFields[i] /= scalar;
      }
      return *this;
    }

    // bx by and bz are projections of beta
    LorentzVectorImpl& Boost(Type bx, Type by, Type bz) {
      Type b2 = bx * bx + by * by + bz * bz;

      if (b2 >= 1) {
        throw std::runtime_error("Boost faster than speed of light.");
      }
      if (b2 <= .95 or not IsSpaceLike()) {
        Type ggamma = 1.0 / std::sqrt(1.0 - b2);
        Type bp = bx * x + by * y + bz * z;
        Type gamma2 = b2 > 0 ? (ggamma - 1.0) / b2 : 0.0;

        x = x + gamma2 * bp * bx + ggamma * bx * t;
        y = y + gamma2 * bp * by + ggamma * by * t;
        z = z + gamma2 * bp * bz + ggamma * bz * t;
        t = ggamma * (t + bp);

        // for big betas use rapidities instead (B = R^-1B'R, where B' is axis boost (Oz here) and R is Rotation
        // matrix for spatial part)
      } else {
        // calculate direction vector coordinates
        Type b1 = std::sqrt(b2);
        Vector3<Type> r_vec{bx / b1, by / b1, bz / b1};
        Vector3<Type> r_back = rotateUz({0, 0, 1}, r_vec);

        // rotate space vector so that boost direction is {0, 0, 1} in new coordinates
        auto new_coord = rotateUz({x, y, z}, r_vec);
        x = new_coord.x, y = new_coord.y, z = new_coord.z;

        BoostAxisRapidity(std::atanh(b1));  // boost along Oz
        // rotate back
        new_coord = rotateUz({x, y, z}, r_back);
        x = new_coord.x, y = new_coord.y, z = new_coord.z;
      }

      return *this;
    }

    // axis from 1 to 3 correspond to x-y-z. Note: this gives correct results only if other axes components are
    // zero. TODO: Throw an error otherwise
    LorentzVectorImpl& BoostAxisRapidity(Type rapidity, uint axis = 3u) {
      if (axis > 3 or axis < 1) {
        throw std::runtime_error("Wrong axis in boostAxis. 1 for x, 2 for y, 3 for z.");
      }
      if (not IsSpaceLike()) {
        throw std::runtime_error("Rapidity calculation only viable for space-like 4-vectors. Use boost() instead");
      }
      Type inv = std::sqrt(e * e - this->*kFields[axis] * this->*kFields[axis]);
      Type r_rapidity = rapidity + .5 * (std::log(e + this->*kFields[axis]) - std::log(e - this->*kFields[axis]));
      e = inv * std::cosh(r_rapidity);
      this->*kFields[axis] = inv * std::sinh(r_rapidity);

      return *this;
    }

    // boost by vector
    LorentzVectorImpl& Boost(const LorentzVectorImpl& target) {
      return boost(target.x / target.e, target.y / target.e, target.z / target.e);
    }

    // extract spatial part
    Vector3<Type> SpatialPart() const {
      Vector3<Type> position;
      position.x = x;
      position.y = y;
      position.z = z;
      return position;
    }

    Type Mag2() const { return t * t - (x * x + y * y + z * z); }
    Type Mag() const { return std::sqrt(Mag2()); }

    bool IsSpaceLike() const { return Mag2() > 0; }
    bool IsLightLike() const { return Mag2() == 0; }
    bool IsTimeLike() const { return Mag2() < 0; }
  };

  template <typename Type>
  LorentzVectorImpl<Type> operator+(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
    auto res = a;
    res += b;
    return res;
  }

  template <typename Type>
  LorentzVectorImpl<Type> operator-(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
    auto res = a;
    res -= b;
    return res;
  }

  // inverse momentum
  template <typename Type> LorentzVectorImpl<Type> operator-(const LorentzVectorImpl<Type>& a) {
    auto res = a;
    res.x = -res.x;
    res.y = -res.y;
    res.z = -res.z;
    return res;
  }

  template <typename Type, typename Scalar>
  LorentzVectorImpl<Type> operator*(const LorentzVectorImpl<Type>& vec, Scalar scalar) {
    auto res = vec;
    res *= scalar;
    return res;
  }

  template <typename Type, typename Scalar>
  LorentzVectorImpl<Type> operator*(Scalar scalar, const LorentzVectorImpl<Type>& vec) {
    return vec * scalar;
  }

  template <typename Type, typename Scalar>
  LorentzVectorImpl<Type> operator/(const LorentzVectorImpl<Type>& vec, Scalar scalar) {
    auto res = vec;
    res /= scalar;
    return res;
  }

  template <typename Type> bool operator==(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
    return a.e == b.e && a.x == b.x && a.y == b.y && a.z == b.z;
  }

  template <typename Type> bool operator!=(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
    return !(a == b);
  }

  template <typename Type> std::ostream& operator<<(std::ostream& out, const LorentzVectorImpl<Type>& vec) {
    out << "(" << vec.e << ", " << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out;
  }
}  // namespace cola

#endif  // COLA_LORENTZVECTOR_HH