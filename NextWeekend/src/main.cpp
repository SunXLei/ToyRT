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

hittable_list random_scene();

//int main()
//{
//	//store file
//	std::ofstream ofs("./image/res.ppm");
//
//	// Image
//	const auto aspect_ratio = 3.0 / 2.0;
//	const int imageWidth = 1200;
//	const int imageHeight = static_cast<int>(imageWidth / aspect_ratio);
//	const int samples_per_pixel = 500;
//	const int max_depth = 50;
//
//	// World
//	auto world = random_scene();
//
//	// Camera
//	point3 lookfrom(13, 2, 3);
//	point3 lookat(0, 0, 0);
//	vec3 vup(0, 1, 0);
//	auto dist_to_focus = 10.0;
//	auto aperture = 0.1;
//
//	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
//
//	// Render
//	ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
//
//	for (int j = imageHeight - 1; j >= 0; --j)
//	{
//		std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
//		for (int i = 0; i < imageWidth; ++i)
//		{
//			color pixel_color(0, 0, 0);
//			for (int s = 0; s < samples_per_pixel; ++s) 
//			{
//				auto u = (i + random_double()) / (imageWidth - 1);
//				auto v = (j + random_double()) / (imageHeight - 1);
//				ray r = cam.get_ray(u, v);
//				pixel_color += RayColor(r, world, max_depth);
//			}
//			WriteColor(ofs, pixel_color, samples_per_pixel);
//		}
//	}
//	std::cerr << "\nDone.\n" << std::flush;
//
//
//	////打印输出当前路径
//	//char *buffer;
//	////也可以将buffer作为输出参数
//	//if ((buffer = _getcwd(NULL, 0)) == NULL)
//	//{
//	//	perror("getcwd error");
//	//}
//	//else
//	//{
//	//	printf("path:%s\n", buffer);
//	//	free(buffer);
//	//}
//
//
//	system("pause");
//	return 0;
//}


hittable_list random_scene() 
{
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

// old main, for defocus blur or depth of field test

int main()
{
	//store file
	std::ofstream ofs("./image/res.ppm");

	// Image
	const auto aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// World
	hittable_list world;
	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	point3 lookfrom(3, 3, 2);
	point3 lookat(-1, 0, -1);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = (lookfrom - lookat).length();
	auto aperture = 2;

	camera cam(lookfrom, lookat, vup, 20, aspectRatio, aperture, dist_to_focus);

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