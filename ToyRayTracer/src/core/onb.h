#pragma once
#include "./math.h"

class ONB {
public:
	ONB() {}

	inline Vec3 operator[](int i) const { return axis[i]; }

	Vec3 u() const { return axis[0]; }
	Vec3 v() const { return axis[1]; }
	Vec3 w() const { return axis[2]; }

	Vec3 Local(double a, double b, double c) const { return a * u() + b * v() + c * w(); }
	Vec3 Local(const Vec3& a) const { return a.x()*u() + a.y()*v() + a.z()*w(); }

	void BuildFromW(const Vec3&);

public:
	Vec3 axis[3];
};

