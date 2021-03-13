#include "./camera.h"

Camera::Camera(Point3 lookfrom, Point3 lookat, Vec3 vup, double vfov,
	double aspect_ratio,
	double aperture,
	double focus_dist)
{
	auto theta = DegreesToRadians(vfov);
	auto h = tan(theta / 2);
	auto viewport_height = 2.0 * h;
	auto viewport_width = aspect_ratio * viewport_height;

	w = UnitVector(lookfrom - lookat);
	u = UnitVector(CrossProduct(vup, w));
	v = CrossProduct(w, u);

	origin = lookfrom;
	horizontal = focus_dist * viewport_width * u;
	vertical = focus_dist * viewport_height * v;
	lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;
	lens_radius = aperture / 2;
}

Ray Camera::GetRay(double s, double t) const
{
	Vec3 rd = lens_radius * RandomInUnitDisk();
	Vec3 offset = u * rd.x() + v * rd.y();
	return Ray(origin + offset,lower_left_corner + s * horizontal + t * vertical - origin - offset);
}