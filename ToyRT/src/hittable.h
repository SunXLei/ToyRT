#pragma once
#include "rtweekend.h"

// forward declaration
class material;

struct hitRecord 
{
	point3 p;
	vec3 normal;
	shared_ptr<material> mat_ptr;
	double t;
	bool isFrontFace;

	inline void SetFaceNormal(const ray& r, const vec3& outwardNormal) 
	{
		isFrontFace = dot(r.direction(), outwardNormal) < 0;
		normal = isFrontFace ? outwardNormal : -outwardNormal;
	}
};

class hittable 
{
public:
	virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const = 0;
};