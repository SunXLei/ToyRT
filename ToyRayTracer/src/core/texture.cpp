#include "./texture.h"
#include "./raw_stb_image.h"

ImageTexture::ImageTexture()
	: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

ImageTexture::ImageTexture(const char* filename)
{
	auto components_per_pixel = bytes_per_pixel;

	data = stbi_load(
		filename, &width, &height, &components_per_pixel, components_per_pixel);

	if (!data) {
		std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
		width = height = 0;
	}

	bytes_per_scanline = bytes_per_pixel * width;
}

ImageTexture::~ImageTexture()
{
	delete data;
}
Color ImageTexture::Value(double u, double v, const Vec3& p) const
{
	// If we have no texture data, then return solid cyan as a debugging aid.
	if (data == nullptr)
		return Color(0, 1, 1);

	// Clamp input texture coordinates to [0,1] x [1,0]
	u = Clamp(u, 0.0, 1.0);
	v = 1.0 - Clamp(v, 0.0, 1.0);  // Flip V to image coordinates

	auto i = static_cast<int>(u * width);
	auto j = static_cast<int>(v * height);

	// Clamp integer mapping, since actual coordinates should be less than 1.0
	if (i >= width)  i = width - 1;
	if (j >= height) j = height - 1;

	const auto color_scale = 1.0 / 255.0;
	auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

	return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
}