#include "./ray.h"

Ray::Ray()
{

}

Ray::Ray(const Point3& origin, const Vec3& direction) : orig(origin), dir(direction)
{
	// normalize
	//dir = UnitVector(dir);
}

Point3 Ray::Origin() const
{
	return orig;
}

Vec3 Ray::Direction() const
{
	return dir;
}

Point3 Ray::At(double t) const
{
	return orig + t * dir;
}

