#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittablelist.h"
#include "material.h"
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


color RayColor(const ray& r, const hittable& world, int depth) 
{
	hitRecord rec;

	if (depth <= 0)
		return color(0, 0, 0);

	// more recursive, more black, the 0.001 is for hitting itself
	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * RayColor(scattered, world, depth - 1);

		return color(0,0,0);
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
	const int imageWidth = 1000;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samples_per_pixel = 300;
	const int max_depth = 50;

	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 1.5, -3.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -2.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), -0.4, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	camera cam(45.0, aspectRatio);

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
				pixel_color += RayColor(r, world, max_depth);
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