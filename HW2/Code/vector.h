//Vector.h
//
//Class representing a vector (vec3 because it has x, y, and z coordinates)
//Includes ray as well

#include <cmath>

#ifndef VECTOR_INCLUDED
#define VECTOR_INCLUDED

struct Vec3
{
    float x, y, z;
    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    const float Magnitude() const // don't want to accidentally change values here
    {
        return sqrt( x*x + y*y + z*z);
    }
    Vec3* UnitVector() const // don't want to accidentally change values here
    {
        const float magnitude = Magnitude();
        return new Vec3(x/magnitude, y/magnitude, z/magnitude);
    }
    Vec3& operator += (const Vec3 &a) // don't want to change a's values
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }
    Vec3& operator *= (const Vec3 &a) // don't want to change a's values
    {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        return *this;
    }
};

Vec3 operator + (const Vec3 &a, const Vec3 &b);
Vec3 operator - (const Vec3 &a, const Vec3 &b);

float dotProd(const Vec3 &a, const Vec3 &b);
Vec3 crossProd(const Vec3 &a, const Vec3 &b);

// We'll also put rays here
struct Ray
{
    Vec3 o; // origin
    Vec3 d; // direction
    Ray(const Vec3& o, const Vec3& d) : o(o), d(d) {}
};

#endif