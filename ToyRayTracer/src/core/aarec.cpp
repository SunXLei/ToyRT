#include "./aarec.h"
// ---XYRect---

bool XYRect::BoundingBox(AABB& output_box) const 
{
	// The bounding box must have non-zero width in each dimension, so pad the Z
	// dimension a small amount.
	output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
	return true;
}

bool XYRect::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	auto t = (k - r.Origin().z()) / r.Direction().z();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.Origin().x() + t * r.Direction().x();
	auto y = r.Origin().y() + t * r.Direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	auto outward_normal = Vec3(0, 0, 1);
	rec.SetFaceNormal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);
	return true;
}

// ---XZRect---

double XZRect::PDFValue(const Point3& origin, const Vec3& v) const
{
	HitRecord rec;
	if (!this->Hit(Ray(origin, v), 0.001, INF, rec))
		return 0;

	auto area = (x1 - x0)*(z1 - z0);
	auto distance_squared = rec.t * rec.t * v.LengthSquared();
	auto cosine = fabs(DotProduct(v, rec.normal) / v.Length());

	return distance_squared / (cosine * area);
}

Vec3 XZRect::Random(const Point3& origin) const
{
	auto random_point = Point3(RandomDouble(x0, x1), k, RandomDouble(z0, z1));
	return random_point - origin;
}

bool XZRect::BoundingBox(AABB& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the Y
	// dimension a small amount.
	output_box = AABB(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
	return true;
}

bool XZRect::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	auto t = (k - r.Origin().y()) / r.Direction().y();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.Origin().x() + t * r.Direction().x();
	auto z = r.Origin().z() + t * r.Direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = Vec3(0, 1, 0);
	rec.SetFaceNormal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);
	return true;
}

// ---YZRect---
 bool YZRect::BoundingBox(AABB& output_box) const
 {
	// The bounding box must have non-zero width in each dimension, so pad the X
	// dimension a small amount.
	output_box = AABB(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
	return true;
}

bool YZRect::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	auto t = (k - r.Origin().x()) / r.Direction().x();
	if (t < t_min || t > t_max)
		return false;
	auto y = r.Origin().y() + t * r.Direction().y();
	auto z = r.Origin().z() + t * r.Direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = Vec3(1, 0, 0);
	rec.SetFaceNormal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.At(t);
	return true;
}