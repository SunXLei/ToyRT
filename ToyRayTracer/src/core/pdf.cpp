#include "./pdf.h"
#include "./hittable.h"

// ---CosPDF---
// cos distribution pdf
CosPDF::CosPDF(const Vec3& w) { uvw.BuildFromW(w); }

double CosPDF::Value(const Vec3& direction) const
{
	auto cosine = DotProduct(UnitVector(direction), uvw.w());
	return (cosine <= 0) ? 0 : cosine / PI;
}

Vec3 CosPDF::Generate() const
{
	return uvw.Local(random_cosine_direction());
}

// ---HittablePDF---
// use the hittable object to help calcualte pdf and sample direction
HittablePDF::HittablePDF(shared_ptr<Hittable> p, const Point3& origin) : ptr(p), o(origin) {}

double HittablePDF::Value(const Vec3& direction) const
{
	return ptr->PDFValue(o, direction);
}

Vec3 HittablePDF::Generate() const
{
	return ptr->Random(o);
}