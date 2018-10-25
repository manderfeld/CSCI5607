#include "vector.h"

Vec3 operator + (const Vec3 &a, const Vec3 &b)
{
	return Vec3( a.x+b.x, a.y+b.y, a.z+b.z);
}
Vec3 operator - (const Vec3 &a, const Vec3 &b)
{
	return Vec3( a.x-b.x, a.y-b.y, a.z-b.z);
}
Vec3 operator * (const float &a, const Vec3 &b)
{
	return Vec3(a*b.x, a*b.y, a*b.z);
}
Vec3 operator / (const Vec3 &b, const float &a)
{
	return Vec3(b.x/a, b.y/a, b.z/a);
}

float dotProd(const Vec3 &a, const Vec3 &b)
{
	return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}
Vec3* crossProd(const Vec3 &a, const Vec3 &b)
{
	return new Vec3(
		a.y*b.z - b.y*a.z,
		a.z*b.x - b.z*a.x,
		a.x*b.y - b.x*a.y
		);
}