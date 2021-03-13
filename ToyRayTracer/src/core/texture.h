#pragma once
#include <iostream>

#include "./math.h"

class Texture
{
public:
	virtual Color Value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture
{
public:
	SolidColor() {}
	SolidColor(Color c) : color_value(c) {}

	SolidColor(double red, double green, double blue)
		: SolidColor(Color(red, green, blue)) {}

	virtual Color Value(double u, double v, const Vec3& p) const override
	{
		return color_value;
	}

private:
	Color color_value;
};

class ImageTexture : public Texture {
public:
	const static int bytes_per_pixel = 3;

	ImageTexture();
	ImageTexture(const char* filename);
	~ImageTexture();

	virtual Color Value(double u, double v, const Vec3& p) const override;

private:
	unsigned char *data;
	int width, height;
	int bytes_per_scanline;
};