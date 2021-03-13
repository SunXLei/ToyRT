#pragma once
#include "./math.h"
#include "./hittable.h"
#include "./materials.h"

class XYRect : public Hittable {
public:
	XYRect() {}

	XYRect(double _x0, double _x1, double _y0, double _y1, double _k,
		shared_ptr<Material> mat)
		: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

public:
	shared_ptr<Material> mp;
	double x0, x1, y0, y1, k;
};

class XZRect : public Hittable {
public:
	XZRect() {}

	XZRect(double _x0, double _x1, double _z0, double _z1, double _k,
		shared_ptr<Material> mat)
		: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

	virtual double PDFValue(const Point3& origin, const Vec3& v) const override;
	virtual Vec3 Random(const Point3& origin) const override;

public:
	shared_ptr<Material> mp;
	double x0, x1, z0, z1, k;
};



class YZRect : public Hittable {
public:
	YZRect() {}

	YZRect(double _y0, double _y1, double _z0, double _z1, double _k,
		shared_ptr<Material> mat)
		: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

public:
	shared_ptr<Material> mp;
	double y0, y1, z0, z1, k;
};

