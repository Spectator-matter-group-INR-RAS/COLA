#ifndef COLA_LORENTZVECTOR_HH
#define COLA_LORENTZVECTOR_HH

#include <array>

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

        // borrowed from CLHEP. bx by and bz are projections of beta
        LorentzVectorImpl& boost(Type bx, Type by, Type bz) {
            Type b2 = bx*bx + by*by + bz*bz;
            if (b2 >= 1)
                throw std::runtime_error("Boost faster than speed of light.");
            Type ggamma = 1.0 / std::sqrt(1.0 - b2);
            Type bp = bx*x + by*y + bz*z;
            Type gamma2 = b2 > 0 ? (ggamma - 1.0)/b2 : 0.0;

            x = x + gamma2*bp*bx + ggamma*bx*t;
            y = y + gamma2*bp*by + ggamma*by*t;
            z = z + gamma2*bp*bz + ggamma*bz*t;
            t = ggamma*(t + bp);

            return *this;
        }

        // axis from 1 to 3 correspond to x-y-z
        LorentzVectorImpl& boostAxis(Type beta, uint axis=3) {
            Type b2 = beta*beta;
            if (axis > 3 or axis < 1) {
                throw std::runtime_error("Wrong axis in boostAxis. 1 for x, 2 for y, 3 for z.");
            }
            if (b2 >= 1)
                throw std::runtime_error("Boost faster than the speed of light.");
            Type ggamma = std::sqrt(1./(1.-b2));
            t = ggamma * (t + beta * Fields_[axis]);
            Fields_[axis] = ggamma * (Fields_[axis] + beta * t);

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
} // namespace cola

#endif // COLA_LORENTZVECTOR_HH
