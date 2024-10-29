#ifndef COLA_LORENTZVECTOR_HH
#define COLA_LORENTZVECTOR_HH

#include <array>
#include <cmath>
#include <iostream>

namespace cola {

    template <typename Type = double>
    struct Vector3 {
        Type x, y, z;
    };

    template <typename Type = double>
    class LorentzVectorImpl {
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

        static inline constexpr std::array<FieldPtr, 4> Fields_ = {
            &LorentzVectorImpl::e,
            &LorentzVectorImpl::x,
            &LorentzVectorImpl::y,
            &LorentzVectorImpl::z
        };

    public:
        const Type& operator[](int i) const { return this->*Fields_[i]; }
        Type& operator[](int i) { return this->*Fields_[i]; }

        LorentzVectorImpl& operator+=(const LorentzVectorImpl& other) {
            for (size_t i = 0; i < Fields_.size(); ++i) {
                this->*Fields_[i] += other.*Fields_[i];
            }
            return *this;
        }

        LorentzVectorImpl& operator-=(const LorentzVectorImpl& other) {
            for (size_t i = 0; i < Fields_.size(); ++i) {
                this->*Fields_[i] -= other.*Fields_[i];
            }
            return *this;
        }

        LorentzVectorImpl& operator*=(Type scalar) {
            for (size_t i = 0; i < Fields_.size(); ++i) {
                this->*Fields_[i] *= scalar;
            }
            return *this;
        }

        LorentzVectorImpl& operator/=(Type scalar) {
            for (size_t i = 0; i < Fields_.size(); ++i) {
                this->*Fields_[i] /= scalar;
            }
            return *this;
        }

        // bx by and bz are projections of beta
        LorentzVectorImpl& boost(Type bx, Type by, Type bz) {

        Type b2 = bx * bx + by * by + bz * bz;

        if (b2 >= 1)
            throw std::runtime_error("Boost faster than speed of light.");
        if (b2 <= .95 or not isSpaceLike()) {
            Type ggamma = 1.0 / std::sqrt(1.0 - b2);
            Type bp = bx * x + by * y + bz * z;
            Type gamma2 = b2 > 0 ? (ggamma - 1.0) / b2 : 0.0;

            x = x + gamma2 * bp * bx + ggamma * bx * t;
            y = y + gamma2 * bp * by + ggamma * by * t;
            z = z + gamma2 * bp * bz + ggamma * bz * t;
            t = ggamma * (t + bp);

            // for big betas use rapidities instead (B = R^-1B'R, where B' is axis boost (Oz here) and R is Rotation matrix for spatial part)
        } else {
            // calculate direction vector coordinates
            Type b1 = std::sqrt(b2);
            Vector3<Type> rVec{bx / b1, by / b1, bz / b1};
            Vector3<Type> rBack = rotateUz({0, 0, 1}, rVec);

            // rotate space vector so that boost direction is {0, 0, 1} in new coordinates
            auto newCoord = rotateUz({x, y, z}, rVec);
            x = newCoord.x, y = newCoord.y, z = newCoord.z;

            boostAxisRapidity(std::atanh(b1)); // boost along Oz
            // rotate back
            newCoord = rotateUz({x, y, z}, rBack);
            x = newCoord.x, y = newCoord.y, z = newCoord.z;
        }

        return *this;
        }

        // axis from 1 to 3 correspond to x-y-z. Note: this gives correct results only if other axes components are zero. TODO: Throw an error otherwise
        LorentzVectorImpl& boostAxisRapidity(Type rapidity, uint axis=3u) {
        if (axis > 3 or axis < 1)
            throw std::runtime_error("Wrong axis in boostAxis. 1 for x, 2 for y, 3 for z.");
        if (not isSpaceLike()) {
            throw std::runtime_error("Rapidity calculation only viable for space-like 4-vectors. Use boost() instead");
        }
        Type inv = std::sqrt(e*e - this->*Fields_[axis]*this->*Fields_[axis]);
        Type rRapidity = rapidity + .5 * (std::log(e + this->*Fields_[axis]) - std::log(e - this->*Fields_[axis]));
        e = inv * std::cosh(rRapidity);
        this->*Fields_[axis] = inv * std::sinh(rRapidity);

        return *this;
        }

        Type mag2() const { return t*t - (x*x + y*y + z*z); }
        Type mag() const { return std::sqrt(mag2()); }

        bool isSpaceLike() const { return mag2() > 0; }
        bool isLightLike() const { return mag2() == 0; }
        bool isTimeLike() const { return mag2() < 0; }
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

    template <typename Type>
    bool operator==(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
        return a.e == b.e && a.x == b.x && a.y == b.y && a.z == b.z;
    }

    template <typename Type>
    bool operator!=(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
        return !(a == b);
    }

    template <typename Type>
    std::ostream& operator<<(std::ostream& out, const LorentzVectorImpl<Type>& vec) {
        out << "(" << vec.e << ", " << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return out;
    }

    template <typename Type>
    Vector3<Type> rotateUz(const Vector3<Type> stVec, const Vector3<Type> uzVec) {
        // NewUzVector must be normalized !

        Vector3<Type> resVec;
        double up = uzVec.x*uzVec.x + uzVec.y*uzVec.y;

        if (up > 0) {
            up = std::sqrt(up);
            resVec.x = (uzVec.x * uzVec.z * stVec.x - uzVec.y * stVec.y) / up + uzVec.x * stVec.z;
            resVec.y = (uzVec.y * uzVec.z * stVec.x + uzVec.x * stVec.y) / up + uzVec.y * stVec.z;
            resVec.z = -up * stVec.x + uzVec.z * stVec.z;
        } else if (uzVec.z < 0.) {
            resVec.x = -stVec.x;
            resVec.y = stVec.y;
            resVec.z = -stVec.z;
        } // phi=0  teta=pi

        return resVec;
    }
} // namespace cola

#endif // COLA_LORENTZVECTOR_HH
