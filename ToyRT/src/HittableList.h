#pragma once

#include "rtweekend.h"
#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable 
{
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	virtual bool hit(
		const ray& r, double tMin, double tMax, hitRecord& rec) const override;

public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const 
{
	hitRecord tempRec;
	bool isHitAnything = false;
	auto closestSoFar = tMax;

	for (const auto& object : objects) 
	{
		if (object->hit(r, tMin, closestSoFar, tempRec))
		{
			isHitAnything = true;
			closestSoFar = tempRec.t;
			rec = tempRec;
		}
	}

	return isHitAnything;
}