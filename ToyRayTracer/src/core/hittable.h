#pragma once
#include <memory>
#include <vector>

#include "./math.h"
#include "./ray.h"
#include "./aabb.h"

// forward declaration
class Material; 

struct HitRecord
{
	Point3 p;
	Vec3 normal;
	shared_ptr<Material> mat_ptr;
	double t;
	double u;
	double v;
	bool is_front_face;

	inline void SetFaceNormal(const Ray& r, const Vec3& outward_normal)
	{
		is_front_face = DotProduct(r.Direction(), outward_normal) < 0;
		normal = is_front_face ? outward_normal : -outward_normal;
	}
};

class Hittable
{
public:
	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
	virtual bool BoundingBox(AABB& output_box) const = 0;

	virtual double PDFValue(const Point3& o, const Vec3& v) const 
	{ 
		return 0.0;
	}

	virtual Vec3 Random(const Vec3& o) const 
	{
		return Vec3(1, 0, 0);
	}
};

class HittableList : public Hittable
{
public:
	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<Hittable> object) { objects.push_back(object); }

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

	virtual double PDFValue(const Point3& o, const Vec3& v) const override;
	virtual Vec3 Random(const Vec3& o) const override;

public:
	std::vector<shared_ptr<Hittable>> objects;
};

class RotateY : public Hittable {
public:
	RotateY(shared_ptr<Hittable> p, double angle);
	virtual bool Hit(
		const Ray& r, double t_min, double t_max, HitRecord& rec) const override;

	virtual bool BoundingBox(AABB& output_box) const override 
	{
		output_box = bbox;
		return hasbox;
	}

public:
	shared_ptr<Hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	AABB bbox;
};

class Translate : public Hittable 
{
public:
	Translate(shared_ptr<Hittable> p, const Vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool Hit(
		const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

public:
	shared_ptr<Hittable> ptr;
	Vec3 offset;
};

class FlipFace : public Hittable {
public:
	FlipFace(shared_ptr<Hittable> p) : ptr(p) {}

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
	virtual bool BoundingBox(AABB& output_box) const override;

public:
	shared_ptr<Hittable> ptr;
};
