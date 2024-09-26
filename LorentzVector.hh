#include <array>

namespace cola {
    template <typename Type = double>
    class VectorImpl {
    private:
        using DataArray = std::array<Type, 3>;

    public:
        using iterator = typename DataArray::iterator;
        using const_iterator = typename DataArray::const_iterator;

        iterator begin() { return data_.begin(); }
        const_iterator begin() const { return data_.cbegin(); }
        const_iterator cbegin() const { return data_.cbegin(); }

        iterator end() { return data_.end(); }
        const_iterator end() const { return data_.cend(); }
        const_iterator cend() const { return data_.cend(); }

        VectorImpl(Type x, Type y, Type z)
            : x(x)
            , y(y)
            , z(z)
        {
            static_assert(std::is_arithmetic_v<Type>, "must be numeric type");
        }

        VectorImpl(const VectorImpl& other) = default;
        VectorImpl(VectorImpl&& other) noexcept = default;

        VectorImpl& operator=(const VectorImpl& other) = default;
        VectorImpl& operator=(VectorImpl&& other) noexcept = default;

        const Type& operator[](int i) const { return data_[i]; }
        Type& operator[](int i) { return data_[i]; }

        VectorImpl& operator+=(const VectorImpl& other) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] += other.data_[i];
            }
            return *this;
        }

        VectorImpl& operator-=(const VectorImpl& other) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] -= other.data_[i];
            }
            return *this;
        }

        VectorImpl& operator*=(Type scalar) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] *= scalar;
            }
            return *this;
        }

        VectorImpl& operator/=(Type scalar) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] /= scalar;
            }
            return *this;
        }

        VectorImpl operator-() const {
            VectorImpl res;
            for (auto i = 0; i < data_.size(); ++i) {
                res.data_[i] = -data_[i];
            }
            return res;
        }

        constexpr size_t size() const { return data_.size(); }

        Type mag2() const { return x*x + y*y + z*z; }
        Type mag() const { return std::sqrt(mag2()); }

        union {
            struct {
                Type x;
                Type y;
                Type z;
            };

            // meant to be private
            DataArray data_;
        };
    };

    template <typename Type>
    VectorImpl<Type> operator+(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        auto res = a;
        res += b;
        return res;
    }

    template <typename Type>
    VectorImpl<Type> operator-(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        auto res = a;
        res -= b;
        return res;
    }

    template <typename Type, typename Scalar>
    VectorImpl<Type> operator*(const VectorImpl<Type>& vec, Scalar scalar) {
        auto res = vec;
        res *= scalar;
        return res;
    }

    template <typename Type, typename Scalar>
    VectorImpl<Type> operator*(Scalar scalar, const VectorImpl<Type>& vec) {
        return vec * scalar;
    }

    template <typename Type, typename Scalar>
    VectorImpl<Type> operator/(const VectorImpl<Type>& vec, Scalar scalar) {
        auto res = vec;
        res /= scalar;
        return res;
    }

    template <typename Type>
    bool operator==(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        return a.data_ == b.data_;
    }

    template <typename Type>
    bool operator!=(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        return a.data_ != b.data_;
    }

    template <typename Type>
    Type Dot(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        Type res = 0;
        for (auto i = 0; i < a.size(); ++i) {
            res += a[i] * b[i];
        }
        return res;
    }

    template <typename Type>
    VectorImpl<Type> Cross(const VectorImpl<Type>& a, const VectorImpl<Type>& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    template <typename Type>
    std::ostream& operator<<(std::ostream& out, const VectorImpl<Type>& vec) {
        out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return out;
    }

    template <typename Type = double>
    class LorentzVectorImpl {
    private:
        using DataArray = std::array<Type, 4>;
        using SpacialVector = VectorImpl<Type>;

    public:
        using iterator = typename DataArray::iterator;
        using const_iterator = typename DataArray::const_iterator;

        iterator begin() { return data_.begin(); }
        const_iterator begin() const { return data_.cbegin(); }
        const_iterator cbegin() const { return data_.cbegin(); }

        iterator end() { return data_.end(); }
        const_iterator end() const { return data_.cend(); }
        const_iterator cend() const { return data_.cend(); }

        LorentzVectorImpl(Type e, Type x, Type y, Type z)
            : e(e)
            , x(x)
            , y(y)
            , z(z)
        {
            static_assert(std::is_arithmetic_v<Type>, "must be numeric type");
        }

        LorentzVectorImpl(Type e, const SpacialVector& vec)
            : e(e)
            , vec(vec)
        {
            static_assert(std::is_arithmetic_v<Type>, "must be numeric type");
        }

        LorentzVectorImpl(const LorentzVectorImpl& other) = default;
        LorentzVectorImpl(LorentzVectorImpl&& other) noexcept = default;

        LorentzVectorImpl& operator=(const LorentzVectorImpl& other) = default;
        LorentzVectorImpl& operator=(LorentzVectorImpl&& other) noexcept = default;

        const Type& operator[](int i) const { return data_[i]; }
        Type& operator[](int i) { return data_[i]; }

        LorentzVectorImpl& operator+=(const LorentzVectorImpl& other) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] += other.data_[i];
            }
            return *this;
        }

        LorentzVectorImpl& operator-=(const LorentzVectorImpl& other) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] -= other.data_[i];
            }
            return *this;
        }

        LorentzVectorImpl& operator*=(Type scalar) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] *= scalar;
            }
            return *this;
        }

        LorentzVectorImpl& operator/=(Type scalar) {
            for (auto i = 0; i < data_.size(); ++i) {
                data_[i] /= scalar;
            }
            return *this;
        }

        constexpr size_t size() const { return data_.size(); }

        Type mag2() const { return t*t - vec.mag2(); }
        Type mag() const { return std::sqrt(mag2()); }

        bool isSpaceLike() const { return mag2() > 0; }
        bool isLightLike() const { return mag2() == 0; }
        bool isTimeLike() const { return mag2() < 0; }

        union {
            struct {
                union {
                    Type e;
                    Type t;
                };
                union {
                    struct {
                        Type x;
                        Type y;
                        Type z;
                    };
                    SpacialVector vec;
                };
            };

            // meant to be private
            DataArray data_;
        };
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
        return a.data_ == b.data_;
    }

    template <typename Type>
    bool operator!=(const LorentzVectorImpl<Type>& a, const LorentzVectorImpl<Type>& b) {
        return a.data_ != b.data_;
    }

    template <typename Type>
    std::ostream& operator<<(std::ostream& out, const LorentzVectorImpl<Type>& vec) {
        out << "(" << vec.e << ", " << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return out;
    }
} // namespace cola
