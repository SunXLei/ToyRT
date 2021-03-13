#include "./math.h"

// Vec3 member function
Vec3::Vec3() : e{ 0,0,0 } {}
Vec3::Vec3(double e0, double e1, double e2) : e{ e0,e1,e2 } {}
Vec3& Vec3::operator/=(const double t) { return *this *= 1 / t; }
Vec3& Vec3::operator*=(const double t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; }
Vec3& Vec3::operator+=(const Vec3 &v) { e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2]; return *this; }
Vec3 Vec3::operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
Vec3 Vec3::Random() { return Vec3(RandomDouble(), RandomDouble(), RandomDouble()); }
Vec3 Vec3::Random(double min, double max) { return Vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max)); }
double Vec3::x() const{ return e[0]; }
double Vec3::y() const { return e[1]; }
double Vec3::z() const { return e[2]; }
double Vec3::operator[](int i) const { return e[i]; }
double& Vec3::operator[](int i) { return e[i]; }
double Vec3::LengthSquared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
double Vec3::Length() const { return sqrt(LengthSquared()); }



