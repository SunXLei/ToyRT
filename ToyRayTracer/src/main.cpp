#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>

#include "core/hittable.h"
#include "core/materials.h"
#include "core/math.h"
#include "core/camera.h"
#include "core/aarec.h"
#include "core/simple_shape.h"
#include "core/pdf.h"
#include "core/bvh.h"

void outputToImage(std::ofstream &out, Color **colorbuffer, int height, int width, int samples_per_pixel);
Color RayTrace(const Ray& r, const Color& background,const Hittable& world, shared_ptr<HittableList> lights, int depth);

// scenes
HittableList CornellBox1();
HittableList CornellBox2();
HittableList NextWeekendFinalScene();

int main()
{
	std::ofstream ofs("./image/res.ppm"); 
	if (!ofs) 
		std::cerr << "ERROR: Could not open image file '" << "./image/res.ppm" << "'.\n";
	

	// image settings
	const auto aspect_ratio = 1.0 / 1.0;
	const int image_width = 500;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 200;
	const int max_depth = 50;

	// camera settings
	Point3 lookfrom;
	Point3 lookat;
	Vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;
	Color background;

	// world
	HittableList world;
	shared_ptr<HittableList> lights;
	switch (1)
	{
	case 1:
		world = CornellBox1();
		lights = make_shared<HittableList>();
		lights->add(make_shared<XZRect>(193, 363, 207, 352, 554, shared_ptr<Material>()));
		lights->add(make_shared<Sphere>(Point3(215.5, 300, 100), 80, shared_ptr<Material>()));
		lookfrom = Point3(278, 278, -800);
		lookat = Point3(278, 278, 0);
		break;
	case 2:
		world = CornellBox2();
		lights = make_shared<HittableList>();
		lights->add(make_shared<XZRect>(153, 403, 187, 372, 554, shared_ptr<Material>()));
		lookfrom = Point3(278, 278, -800);
		lookat = Point3(278, 278, 0);
		break;

	case 3:
		world = NextWeekendFinalScene();
		lights = make_shared<HittableList>();
		lights->add(make_shared<XZRect>(123, 423, 147, 412, 554, shared_ptr<Material>()));
		lookfrom = Point3(478, 278, -600);
		lookat = Point3(278, 278, 0);
		break;

	default:
		break;

	}

	// create camera
	Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

	Color** colorbuffer = new Color*[image_height];
	for (int i = 0; i < image_height; ++i)
		colorbuffer[i] = new Color[image_width];

	// Render
	ofs << "P3\n" << image_width << " " << image_height << "\n255\n";

	clock_t start, end;
	start = clock();

	std::cerr << "running..." << std::flush;
#pragma omp parallel for num_threads(8)
	for (int j = image_height - 1; j >= 0; --j)
	{
		//std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			Color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (i + RandomDouble()) / (image_width - 1);
				auto v = (j + RandomDouble()) / (image_height - 1);
				Ray r = cam.GetRay(u, v);
				pixel_color += RayTrace(r, background, world, lights, max_depth);
			}
			colorbuffer[image_height - 1 - j][i] = pixel_color;
		}
	}

	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	std::cerr << "\nruntime:" << time << "s" << std::flush;
	outputToImage(ofs, colorbuffer, image_height, image_width, samples_per_pixel);
	std::cerr << "\nDone.\n" << std::flush;

	for (int i = 0; i < image_height; ++i)
		delete[] colorbuffer[i];
	delete[] colorbuffer;

	system("pause");
	return 0;
}

Color RayTrace(const Ray& r, const Color& background,
	const Hittable& world, shared_ptr<HittableList> lights, int depth)
{
	HitRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.Hit(r, 0.001, INF, rec))
		return background;

	ScatterRecord srec;
	Color emitted = rec.mat_ptr->Emitted(r, rec, rec.u, rec.v, rec.p);
	if (!rec.mat_ptr->Scatter(r, rec, srec))
		return emitted;

	if (srec.is_specular) {
		return srec.attenuation
			* RayTrace(srec.specular_ray, background, world, lights, depth - 1);
	}

	auto light_ptr = make_shared<HittablePDF>(lights, rec.p);
	MixturePDF p(light_ptr, srec.pdf_ptr);

	Ray scattered = Ray(rec.p, p.Generate());
	auto pdf_val = p.Value(scattered.Direction());

	return emitted
		+ srec.attenuation * rec.mat_ptr->ScatteringPDF(r, rec, scattered)
		* RayTrace(scattered, background, world, lights, depth - 1) / pdf_val;
}

HittableList CornellBox1()
{
	HittableList objects;

	auto red = make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = make_shared<Lambertian>(Color(.22, .45, .15));
	auto blue = make_shared<Lambertian>(Color(.12, .42, .75));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.add(make_shared<YZRect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<FlipFace>(make_shared<XZRect>(193, 363, 207, 352, 554, light)));// 光源的frontface要翻转一下，adapter模式

	objects.add(make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<XYRect>(0, 555, 0, 555, 555, white));

	shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 220, 165), blue);
	box1 = make_shared<RotateY>(box1, -18);
	box1 = make_shared<Translate>(box1, Vec3(130, 0, 65));
	objects.add(box1);	
	auto glass = make_shared<Dielectric>(1.5);
	objects.add(make_shared<Sphere>(Point3(215.5, 300, 130), 80, glass));

	shared_ptr<Material> aluminum = make_shared<Metal>(Color(0.8, 0.85, 0.85), 0.0);
	shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 300, 165), aluminum);
	box2 = make_shared<RotateY>(box2, 15);
	box2 = make_shared<Translate>(box2, Vec3(300, 0, 295));
	objects.add(box2);

	return objects;
}

HittableList CornellBox2()
{
	HittableList objects;

	auto red = make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = make_shared<Lambertian>(Color(.22, .45, .15));
	auto blue = make_shared<Lambertian>(Color(.12, .42, .75));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.add(make_shared<YZRect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<FlipFace>(make_shared<XZRect>(153, 403, 187, 372, 554, light)));// 光源的frontface要翻转一下，adapter模式

	objects.add(make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<XYRect>(0, 555, 0, 555, 555, white));

	shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<RotateY>(box1, 15);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));

	shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));

	auto boundary = make_shared<Sphere>(Point3(185, 235, 195), 70, make_shared<Dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<ConstantMedium>(boundary, 0.1, Color(0.25, 0.75, 0.4)));

	objects.add(make_shared<ConstantMedium>(box1, 0.01, Color(0.9, 0.9, 0.9)));
	objects.add(make_shared<ConstantMedium>(box2, 0.01, Color(0.2, 0.4, 0.9)));

	return objects;
}


HittableList NextWeekendFinalScene()
{
	HittableList boxes1;
	auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

	// ground box
	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) 
	{
		for (int j = 0; j < boxes_per_side; j++) 
		{
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = RandomDouble(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	HittableList objects;

	objects.add(make_shared<BVHNode>(boxes1, 0, 1));

	// top light
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
	objects.add(make_shared<FlipFace>(make_shared<XZRect>(123, 423, 147, 412, 554, light)));

	// sphere
	auto center = Point3(400, 400, 200);
	auto moving_sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
	objects.add(make_shared<Sphere>(center, 50, moving_sphere_material));

	// glass sphere and metal sphere
	objects.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
	objects.add(make_shared<Sphere>(Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

	// subsurface
	auto boundary = make_shared<Sphere>(Point3(360, 170, 145), 70, make_shared<Dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));

	// huge fog
	boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
	objects.add(make_shared<ConstantMedium>(boundary, .0001, Color(0.1, 0.1, 0.1)));

	auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("./src/resource/earthmap.jpg"));
	objects.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));

	objects.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Metal>(Color(0.8, 0.88, 0.85), 0.0)));

	HittableList boxes2;
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<Sphere>(Point3::Random(0, 165), 10, white));
	}

	objects.add(make_shared<Translate>(
		make_shared<RotateY>(
			make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
		Vec3(-100, 270, 395)
		)
	);

	return objects;
}

void outputToImage(std::ofstream &out, Color **colorbuffer, int height, int width, int samples_per_pixel)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			auto r = colorbuffer[i][j].x();
			auto g = colorbuffer[i][j].y();
			auto b = colorbuffer[i][j].z();

			if (r != r) r = 0.0;
			if (g != g) g = 0.0;
			if (b != b) b = 0.0;

			// Divide the color by the number of samples.
			auto scale = 1.0 / samples_per_pixel;
			r = sqrt(r * scale);
			g = sqrt(g * scale);
			b = sqrt(b * scale);

			// Write the translated [0,255] value of each color component.
			out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' '
				<< static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
				<< static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << '\n';
		}
	}
}