
#ifndef AIRBRAKES_SDK_VECMATH_H_
#define AIRBRAKES_SDK_VECMATH_H_

namespace sdk {

using real = float;

/** 3-element vector type */
struct vec3 {
    real x, y, z;

    inline void operator+=(const vec3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }

    inline vec3 operator+(const vec3 &rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    inline vec3 operator-(const vec3 &rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    inline vec3 operator*(const real &rhs) const
    {
        return { x * rhs, y * rhs, z * rhs };
    }

    inline real dot(const vec3 &rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    inline real magnitude_sqr() const {
        return dot(*this);
    }
};

inline vec3 operator*(const real &lhs, const vec3 &rhs)
{
    return rhs * lhs;
}

/** 4-element vector type */
struct vec4 {
    real x, y, z, w;
};


} // namespace sdk

#endif // AIRBRAKES_SDK_VECMATH_H_
