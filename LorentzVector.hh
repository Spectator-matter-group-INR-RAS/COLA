#ifndef COLA_LORENTZVECTOR_HH
#define COLA_LORENTZVECTOR_HH

#include <array>
#include <cmath>
#include <iostream>

namespace cola {
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
        LorentzVectorImpl& boost(Type bx, Type by, Type bz);

        // axis from 1 to 3 correspond to x-y-z. Note: this gives correct results only if other axes components are zero. TODO: Throw an error otherwise
        LorentzVectorImpl& boostAxisRapidity(Type rapidity, uint axis=3);

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
} // namespace cola

#endif // COLA_LORENTZVECTOR_HH
