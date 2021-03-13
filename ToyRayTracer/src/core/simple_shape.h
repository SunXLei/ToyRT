#pragma once
#include "./math.h"
#include "./aarec.h"
#include "./hittable.h"
#include "./materials.h"

class Box : public Hittable {
public:
	Box() {}
	Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr);

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

public:
	Point3 box_min;
	Point3 box_max;
	HittableList sides;
};

class Sphere : public Hittable
{
public:
	Sphere() {}
	Sphere(Point3 cen, double r, shared_ptr<Material> m)
		: center(cen), radius(r), mat_ptr(m) {};

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;
	double PDFValue(const Point3& o, const Vec3& v) const override;
	Vec3 Random(const Point3& o) const override;

public:
	Point3 center;
	double radius;
	shared_ptr<Material> mat_ptr;

private:
	static void GetSphereUV(const Point3& p, double& u, double& v) 
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}
};

class ConstantMedium : public Hittable
{
public:
	ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<Texture> a)
		: boundary(b),
		neg_inv_density(-1 / d),
		phase_function(make_shared<Isotropic>(a))
	{}

	ConstantMedium(shared_ptr<Hittable> b, double d, Color c)
		: boundary(b),
		neg_inv_density(-1 / d),
		phase_function(make_shared<Isotropic>(c))
	{}

	virtual bool Hit(
		const Ray& r, double t_min, double t_max, HitRecord& rec) const override;

	virtual bool BoundingBox(AABB& output_box) const override
	{
		return boundary->BoundingBox(output_box);
	}

public:
	shared_ptr<Hittable> boundary;
	shared_ptr<Material> phase_function;
	double neg_inv_density;
};

