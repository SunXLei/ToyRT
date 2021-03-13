#pragma once
#include "./math.h"
#include "./ray.h"
#include "./pdf.h"
#include "./texture.h"
#include "./hittable.h"

struct ScatterRecord {
	Ray specular_ray;
	bool is_specular;
	Color attenuation;
	shared_ptr<PDF> pdf_ptr;
};

class Material {
public:
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, ScatterRecord& srec) const 
	{
		return false;
	}

	virtual double ScatteringPDF(
		const Ray& r_in, const HitRecord& rec, const Ray& scattered
	) const 
	{
		return 0;
	}

	virtual Color Emitted(const Ray& r_in, const HitRecord& rec, double u, double v,
		const Point3& p) const
	{
		return Color(0, 0, 0);
	}
};

class Lambertian : public Material
{
public:
	Lambertian(const Color& a) : albedo(make_shared<SolidColor>(a)) {}
	Lambertian(shared_ptr<Texture> a) : albedo(a) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, ScatterRecord& srec) const override
	{
		srec.is_specular = false;
		srec.attenuation = albedo->Value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<CosPDF>(rec.normal);
		return true;
	}

	double ScatteringPDF(
		const Ray& r_in, const HitRecord& rec, const Ray& scattered
	) const 
	{
		auto cosine = DotProduct(rec.normal, UnitVector(scattered.Direction()));
		return cosine < 0 ? 0 : cosine / PI;
	}

public:
	shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
	Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
	virtual bool Scatter(
		const Ray& r_in, const HitRecord& rec, ScatterRecord& srec
	) const override 
	{
		Vec3 reflected = Reflect(UnitVector(r_in.Direction()), rec.normal);
		srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		srec.attenuation = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		return true;
	}
public:
	Color albedo;
	double fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool Scatter(
		const Ray& r_in, const HitRecord& rec, ScatterRecord& srec
	) const override
	{
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		srec.attenuation = Color(1.0, 1.0, 1.0);
		double refraction_ratio = rec.is_front_face ? (1.0 / ir) : ir;

		Vec3 unit_direction = UnitVector(r_in.Direction());
		double cos_theta = fmin(DotProduct(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		Vec3 direction;

		// use probability to seperate the reflect and refract part, it's OK
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > RandomDouble())
			direction = Reflect(unit_direction, rec.normal);
		else
			direction = Refract(unit_direction, rec.normal, refraction_ratio);

		srec.specular_ray = Ray(rec.p, direction);
		return true;
	}



public:
	double ir; // Index of Refraction


private:
	static double reflectance(double cosine, double ref_idx)
	{
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}
};

class DiffuseLight : public Material 
{
public:
	DiffuseLight(shared_ptr<Texture> a) : emit(a) {}
	DiffuseLight(Color c) : emit(make_shared<SolidColor>(c)) {}

	virtual Color Emitted(const Ray& r_in, const HitRecord& rec, double u, double v, const Point3& p) const override
	{
		if (rec.is_front_face)
			return emit->Value(u, v, p);
		else
			return Color(0, 0, 0);
	}

public:
	shared_ptr<Texture> emit;
};


class Isotropic : public Material {
public:
	Isotropic(Color c) : albedo(make_shared<SolidColor>(c)) {}
	Isotropic(shared_ptr<Texture> a) : albedo(a) {}

	virtual bool Scatter(
		const Ray& r_in, const HitRecord& rec, ScatterRecord& srec
	) const override {
		srec.is_specular = true;
		srec.specular_ray = Ray(rec.p, random_in_unit_sphere());
		srec.attenuation = albedo->Value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = nullptr;
		return true;
	}

public:
	shared_ptr<Texture> albedo;
};