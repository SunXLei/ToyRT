#include <cmath>
#include "./aabb.h"

AABB::AABB() {}
AABB::AABB(const Point3& a, const Point3& b) { minimum = a; maximum = b; }

Point3 AABB::min() const { return minimum; }
Point3 AABB::max() const { return maximum; }

AABB SurroundingBox(AABB box0, AABB box1)
{
	// �ҵ�2��������С���Ǹ�x,y,z
	Point3 small(fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z()));

	// �ҵ�2�����������Ǹ�x,y,z
	Point3 big(fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z()));

	return AABB(small, big);
}