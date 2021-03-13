#include "./hittable.h"

#include "./ray.h"
#include "./aabb.h"

// ---HittableList---

bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	HitRecord temp_rec;
	bool is_hit_anything = false;
	auto closestSoFar = tMax;

	for (const auto& object : objects)
	{
		if (object->Hit(r, tMin, closestSoFar, temp_rec))
		{
			is_hit_anything = true;
			closestSoFar = temp_rec.t;
			rec = temp_rec;
		}
	}

	return is_hit_anything;
}

bool HittableList::BoundingBox(AABB& output_box) const
{
	if (objects.empty()) return false;

	AABB temp_box;
	bool first_box = true;

	// hitabble list的盒子就是能包住所有object的盒子
	for (const auto& object : objects)
	{
		if (!object->BoundingBox(temp_box)) return false;
		output_box = first_box ? temp_box : SurroundingBox(output_box, temp_box);
		first_box = false;
	}

	return true;
}

double HittableList::PDFValue(const Point3& o, const Vec3& v) const
{
	auto weight = 1.0 / objects.size();
	auto sum = 0.0;

	for (const auto& object : objects)
		sum += weight * object->PDFValue(o, v);

	return sum;
}

Vec3 HittableList::Random(const Vec3& o) const
{
	auto int_size = static_cast<int>(objects.size());
	return objects[RandomInt(0, int_size - 1)]->Random(o);
}

// ---Translate---
bool Translate::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	// 想把物体往某个方向移动，可以从光线的角度考虑，把光线的出发点往相反的方向移动，效果是一样的，只不过
	// 碰撞点要加上这个offset，做假就要做全
	Ray moved_r(r.Origin() - offset, r.Direction());
	if (!ptr->Hit(moved_r, t_min, t_max, rec))
		return false;

	rec.p += offset;
	rec.SetFaceNormal(moved_r, rec.normal);

	return true;
}

bool Translate::BoundingBox(AABB& output_box) const {
	if (!ptr->BoundingBox(output_box))
		return false;

	output_box = AABB(output_box.min() + offset, output_box.max() + offset);
	return true;
}

// ---RotateY---

RotateY::RotateY(shared_ptr<Hittable> p, double angle) : ptr(p) 
{
	auto radians = DegreesToRadians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->BoundingBox(bbox);

	Point3 min(INF, INF, INF);
	Point3 max(-INF, -INF, -INF);

	// 遍历aabb的8个顶点，旋转之后更新aabb
	for (int i = 0; i < 2; i++) 
	{
		for (int j = 0; j < 2; j++) 
		{
			for (int k = 0; k < 2; k++)
			{
				auto x = i * bbox.max().x() + (1 - i)*bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j)*bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k)*bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				Vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) 
				{
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = AABB(min, max);
}

bool RotateY::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	auto origin = r.Origin();
	auto direction = r.Direction();

	// 原点和方向都要方向旋转
	origin[0] = cos_theta * r.Origin()[0] - sin_theta * r.Origin()[2];
	origin[2] = sin_theta * r.Origin()[0] + cos_theta * r.Origin()[2];

	direction[0] = cos_theta * r.Direction()[0] - sin_theta * r.Direction()[2];
	direction[2] = sin_theta * r.Direction()[0] + cos_theta * r.Direction()[2];

	Ray rotated_r(origin, direction);

	if (!ptr->Hit(rotated_r, t_min, t_max, rec))
		return false;

	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.SetFaceNormal(rotated_r, normal);

	return true;
}

// ---FlipFace---

bool FlipFace::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	if (!ptr->Hit(r, t_min, t_max, rec))
		return false;

	rec.is_front_face = !rec.is_front_face;
	return true;
}

bool FlipFace::BoundingBox(AABB& output_box) const 
{
	return ptr->BoundingBox(output_box);
}