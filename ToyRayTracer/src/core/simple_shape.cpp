#include "./simple_shape.h"

#include "./onb.h"

// ---Box---

bool Box::BoundingBox(AABB& output_box) const 
{
	output_box = AABB(box_min, box_max);
	return true;
}

Box::Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr) {
	box_min = p0;
	box_max = p1;

	sides.add(make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

	sides.add(make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	sides.add(make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

	sides.add(make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	sides.add(make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool Box::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	return sides.Hit(r, t_min, t_max, rec);
}

// ---Sphere---

bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	Vec3 oc = r.Origin() - center;
	auto a = r.Direction().LengthSquared();
	auto half_b = DotProduct(oc, r.Direction());
	auto c = oc.LengthSquared() - radius * radius;

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
	rec.p = r.At(rec.t);
	Vec3 outwardNormal = (rec.p - center) / radius;
	rec.SetFaceNormal(r, outwardNormal);
	GetSphereUV(outwardNormal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool Sphere::BoundingBox(AABB& output_box) const
{
	// 普通球直接返回左下角和右上角构成的aabb就行，很简单
	output_box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
	return true;
}

double Sphere::PDFValue(const Point3& o, const Vec3& v) const 
{
	HitRecord rec;
	if (!this->Hit(Ray(o, v), 0.001, INF, rec))
		return 0;

	auto cos_theta_max = sqrt(1 - radius * radius / (center - o).LengthSquared());
	auto solid_angle = 2 * PI * (1 - cos_theta_max);

	return  1 / solid_angle;
}

Vec3 Sphere::Random(const Point3& o) const 
{
	Vec3 direction = center - o;
	auto distance_squared = direction.LengthSquared();
	ONB uvw;
	uvw.BuildFromW(direction);
	return uvw.Local(random_to_sphere(radius, distance_squared));
}

// ---ConstantMedium---

bool ConstantMedium::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const
{
	// Print occasional samples when debugging. To enable, set enableDebug true.
	const bool enableDebug = false;
	const bool debugging = enableDebug && RandomDouble() < 0.00001;

	HitRecord rec1, rec2;

	if (!boundary->Hit(r, -INF, INF, rec1))
		return false;

	if (!boundary->Hit(r, rec1.t + 0.0001, INF, rec2))
		return false;

	if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

	if (rec1.t < t_min) rec1.t = t_min;
	if (rec2.t > t_max) rec2.t = t_max;

	// 光线是不是在物体内有一段时间
	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	// 任意走一段距离
	const auto ray_length = r.Direction().Length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = neg_inv_density * log(RandomDouble()); // 浓度越大越难走出来

	// 如果走出了物体外，直接返回false，没有击中,正是因为没有击中，光线就穿了过去，就能看见后面的东西了
	if (hit_distance > distance_inside_boundary)
		return false;

	// 没有走出物体外，击中了雾
	rec.t = rec1.t + hit_distance / ray_length;
	rec.p = r.At(rec.t);

	if (debugging) 
	{
		std::cerr << "hit_distance = " << hit_distance << '\n'
			<< "rec.t = " << rec.t << '\n'
			<< "rec.p = " << rec.p << '\n';
	}

	rec.normal = Vec3(1, 0, 0);  // arbitrary
	rec.is_front_face = true;     // also arbitrary
	rec.mat_ptr = phase_function;

	return true;
}
