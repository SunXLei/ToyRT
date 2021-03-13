#pragma once
#include "./math.h"

class Ray
{
public:
	Ray();
	Ray(const Point3& origin, const Vec3& direction);

	Point3 Origin() const ;
	Vec3 Direction() const;
	Point3 At(double t) const;

public:
	Point3 orig;
	Vec3 dir;
};