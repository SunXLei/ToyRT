#pragma once
#include "./math.h"
#include "./ray.h"

class Camera
{
public:
	// vertical field-of-view in degrees
	// only the sphere on the focus plane is clear
	Camera(Point3 lookfrom, Point3 lookat, Vec3 vup, double vfov,double aspect_ratio,double aperture,double focus_dist);
	Ray GetRay(double s, double t) const;

private:
	Point3 origin;
	Point3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 u, v, w;
	double lens_radius;
};