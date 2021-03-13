#pragma once
#include <algorithm>

#include "./math.h"
#include "./hittable.h"

inline bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis)
{
	AABB box_a;
	AABB box_b;

	if (!a->BoundingBox( box_a) || !b->BoundingBox(box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis];
}


inline bool box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return box_compare(a, b, 0);
}

inline bool box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return box_compare(a, b, 1);
}

inline bool box_z_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return box_compare(a, b, 2);
}

class BVHNode : public Hittable
{
public:
	BVHNode();

	BVHNode(const HittableList& list, double time0, double time1)
		: BVHNode(list.objects, 0, list.objects.size(), time0, time1)
	{}

	BVHNode(const std::vector<shared_ptr<Hittable>>& src_objects,
		size_t start, size_t end, double time0, double time1);

	virtual bool Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;

	virtual bool BoundingBox(AABB& output_box) const override;

public:
	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;
	AABB box;
};

