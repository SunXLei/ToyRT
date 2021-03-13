#pragma once
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

class Vec3
{
public:
	Vec3();
	Vec3(double e0, double e1, double e2);

	double x() const;
	double y() const;
	double z() const;


	Vec3 operator-() const;
	double operator[](int i) const;
	double& operator[](int i);

	Vec3& operator+=(const Vec3 &v);
	Vec3& operator*=(const double t);
	Vec3& operator/=(const double t);

	double Length() const;
	double LengthSquared() const;

	static Vec3 Random();
	static Vec3 Random(double min, double max);

public:
	double e[3];
};

// Type aliases for Vec3
using Point3 = Vec3;   // 3D point
using Color = Vec3;    // RGB color

// Vec3 Utility Functions
inline std::ostream& operator<<(std::ostream &out, const Vec3 &v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v)
{
	return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t)
{
	return t * v;
}

inline Vec3 operator/(Vec3 v, double t)
{
	return (1 / t) * v;
}

inline double DotProduct(const Vec3 &u, const Vec3 &v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 CrossProduct(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 UnitVector(Vec3 v)
{
	return v / v.Length();
}

inline Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * DotProduct(v, n)*n;
}

inline Vec3 Refract(const Vec3& in, const Vec3& n, double etai_over_etat)
{
	auto cos_theta = fmin(DotProduct(-in, n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (in + cos_theta * n);
	Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.LengthSquared())) * n;
	return r_out_perp + r_out_parallel;
}

// Math Utility Functions
inline double DegreesToRadians(double degrees)
{
	return degrees * PI / 180.0;
}

inline double RandomDouble()
{
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
}

inline double RandomDouble(double min, double max)
{
	// Returns a random real in [min,max).
	return min + (max - min)*RandomDouble();
}

inline int RandomInt(int min, int max)
{
	// Returns a random integer in [min,max].
	return static_cast<int>(RandomDouble(min, max + 1));
}

inline double Clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// sample functions
inline Vec3 RandomInUnitDisk()
{
	while (true)
	{
		auto p = Vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
		if (p.LengthSquared() >= 1) continue;
		return p;
	}
}


inline Vec3 random_in_unit_sphere()
{
	while (true)
	{
		auto p = Vec3::Random(-1, 1);
		if (p.Length() >= 1) continue;
		return p;
	}
}

inline Vec3 random_cosine_direction() 
{
	auto r1 = RandomDouble();
	auto r2 = RandomDouble();
	auto z = sqrt(1 - r2);

	auto phi = 2 * PI*r1;
	auto x = cos(phi)*sqrt(r2);
	auto y = sin(phi)*sqrt(r2);

	return Vec3(x, y, z);
}

inline Vec3 random_to_sphere(double radius, double distance_squared)
{
	auto r1 = RandomDouble();
	auto r2 = RandomDouble();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

	auto phi = 2 * PI*r1;
	auto x = cos(phi)*sqrt(1 - z * z);
	auto y = sin(phi)*sqrt(1 - z * z);

	return Vec3(x, y, z);
}