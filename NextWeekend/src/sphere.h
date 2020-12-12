#pragma once
#include "rtweekend.h"

#include "hittable.h"

class sphere : public hittable 
{
public:
	sphere() {}
	sphere(point3 cen, double r, shared_ptr<material> m)
		: center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const 
{
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	// Check the two results in order (smaller first)
	auto root = (-half_b - sqrtd) / a;
	if (root < tMin || root > tMax) 
	{
		root = (-half_b + sqrtd) / a;
		if (root < tMin || root > tMax)
			return false;
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 outwardNormal = (rec.p - center) / radius;
	rec.SetFaceNormal(r, outwardNormal);
	rec.mat_ptr = mat_ptr;

	return true;
}