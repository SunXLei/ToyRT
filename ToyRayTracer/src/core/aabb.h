#pragma once
#include <algorithm>

#include "./math.h"
#include "./ray.h"


class AABB
{
public:
	AABB();
	AABB(const Point3& a, const Point3& b);

	Point3 min() const;
	Point3 max() const; 

	// optimized hit function
	inline bool Hit(const Ray& r, double t_min, double t_max) const
	{
		for (int a = 0; a < 3; a++)
		{
			auto invD = 1.0f / r.Direction()[a];
			auto t0 = (min()[a] - r.Origin()[a]) * invD;
			auto t1 = (max()[a] - r.Origin()[a]) * invD;
			if (invD < 0.0f)	// 相对于当前的这两个slab，是反方向传播，那就交换t0,t1
				std::swap(t0, t1);

			// t_min可以保证t_max最终要>0才行，不会出现反向光线也返回true的情况
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min)
				return false;
		}
		return true;
	}

private:
	Point3 minimum;
	Point3 maximum;
};

AABB SurroundingBox(AABB box0, AABB box1);