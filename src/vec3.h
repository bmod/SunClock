#pragma once

#include <iostream>
#include <cmath>

const float kInfinity = std::numeric_limits<float>::max();

template<typename T>
class Vec3 {
public:
    Vec3() : x(0), y(0), z(0) {}

    Vec3(T xx) : x(xx), y(xx), z(xx) {}

    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

    Vec3 operator*(const T& r) const { return Vec3(x * r, y * r, z * r); }

    Vec3 operator*(const Vec3<T>& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

    template<typename U>
    Vec3 operator/(const Vec3<U>& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }

    friend Vec3 operator/(const T r, const Vec3& v) {
        return Vec3(r / v.x, r / v.y, r / v.z);
    }

    const T& operator[](size_t i) const { return (&x)[i]; }

    T& operator[](size_t i) { return (&x)[i]; }

    T length2() const { return x * x + y * y + z * z; }

    T length() const { return std::sqrt(length2()); }

    Vec3& operator+=(const Vec3<T>& v) {
        x += v.x, y += v.y, z += v.z;
        return *this;
    }

    Vec3& operator*=(const float& r) {
        x *= r, y *= r, z *= r;
        return *this;
    }

    friend Vec3 operator*(const float& r, const Vec3& v) {
        return Vec3(v.x * r, v.y * r, v.z * r);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
        os << v.x << " " << v.y << " " << v.z << std::endl;
        return os;
    }

    T x, y, z;
};

template<typename T>
void normalize(Vec3<T>& vec) {
    T len2 = vec.length2();
    if (len2 > 0) {
        T invLen = 1 / std::sqrt(len2);
        vec.x *= invLen, vec.y *= invLen, vec.z *= invLen;
    }
}

template<typename T>
T dot(const Vec3<T>& va, const Vec3<T>& vb) {
    return va.x * vb.x + va.y * vb.y + va.z * vb.z;
}

using Vec3f = Vec3<float>;
