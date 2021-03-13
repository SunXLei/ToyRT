#include "./onb.h"
#include "./math.h"

void ONB::BuildFromW(const Vec3& n)
{
	axis[2] = UnitVector(n);
	Vec3 a = (fabs(w().x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
	axis[1] = UnitVector(CrossProduct(w(), a));
	axis[0] = CrossProduct(w(), v());
}