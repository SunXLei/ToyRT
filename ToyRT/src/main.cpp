#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittablelist.h"
#include "sphere.h"

#include <iostream>
#include <fstream>


//b^2 - 4ac, (P-C)(P-C) = r^2, P = A + Bt
double HitSphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	
	if (discriminant < 0)
		return -1;
	else
		return (-half_b - sqrt(discriminant)) / a;
}


color RayColor(const ray& r, const hittable& world) 
{
	hitRecord rec;

	if (world.hit(r, 0, infinity, rec)) 
	{
		return 0.5 * (rec.normal + color(1, 1, 1));
	}

	vec3 unitDirection = unit_vector(r.direction());
	auto t = 0.5*(unitDirection.y() + 1.0);
	return (1.0 - t)*color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main()
{
	//store file
	std::ofstream ofs("./image/res.ppm");

	// Image
	const auto aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samples_per_pixel = 100;

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

	// Camera
	camera cam;

	// Render
	ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

	for (int j = imageHeight - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
		for (int i = 0; i < imageWidth; ++i)
		{
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) 
			{
				auto u = (i + random_double()) / (imageWidth - 1);
				auto v = (j + random_double()) / (imageHeight - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += RayColor(r, world);
			}
			WriteColor(ofs, pixel_color, samples_per_pixel);
		}
	}
	std::cerr << "\nDone.\n" << std::flush;


	////打印输出当前路径
	//char *buffer;
	////也可以将buffer作为输出参数
	//if ((buffer = _getcwd(NULL, 0)) == NULL)
	//{
	//	perror("getcwd error");
	//}
	//else
	//{
	//	printf("path:%s\n", buffer);
	//	free(buffer);
	//}


	system("pause");
	return 0;
}