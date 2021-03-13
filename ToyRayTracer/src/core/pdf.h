#pragma once
#include "./math.h"
#include "./onb.h"
#include "./hittable.h"

class PDF 
{
public:
	virtual ~PDF() {}

	virtual double Value(const Vec3& direction) const = 0;
	virtual Vec3 Generate() const = 0;
};


class CosPDF : public PDF 
{
public:
	CosPDF(const Vec3& w);

	// pdf value
	virtual double Value(const Vec3& direction) const override;

	// sample a direction according to cos distribution
	virtual Vec3 Generate() const override;

public:
	ONB uvw;
};

class HittablePDF : public PDF {
public:
	HittablePDF(shared_ptr<Hittable> p, const Point3& origin);

	virtual double Value(const Vec3& direction) const override;

	virtual Vec3 Generate() const override;

public:
	Point3 o;
	shared_ptr<Hittable> ptr;
};

class MixturePDF : public PDF {
public:
	MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual double Value(const Vec3& direction) const override {
		return 0.5 * p[0]->Value(direction) + 0.5 *p[1]->Value(direction);
	}

	// 这种光线发出的选择确实也符合混合之后的PDF
	virtual Vec3 Generate() const override {
		if (RandomDouble() < 0.5)
			return p[0]->Generate();
		else
			return p[1]->Generate();
	}

public:
	shared_ptr<PDF> p[2];
};