#ifndef _DRAWING_
#define _DRAWING_
#include <math.h>
#include <iostream>
#include <limits>
#include <exception>
#include <random>
#include "tgaimage.h"
#include "geometry.h"

#define DRAW_BOUNDIND_BOX false
#define DRAW_EDGES false

struct Zbuffer
{
	uint32_t ** buffer;
	size_t width;
	size_t height;
	Zbuffer(size_t width, size_t height) : width(width), height(height)
	{
		buffer = new uint32_t*[height];
		for(size_t i = 0; i < height; i++)
			buffer[i] = new uint32_t[width];

		for(size_t x = 0; x < width; x++)
			for(size_t y = 0; y < height; y++)
				buffer[x][y] = std::numeric_limits<uint32_t>::min(); //buffer[x][y] = std::numeric_limits<uint32_t>::max();
	};
	~Zbuffer()
	{
		for(size_t i = 0; i < height; i++)
			delete[] buffer[i];
		delete buffer;
	};
	uint32_t &operator() (uint32_t x, uint32_t y)
	{
		if(x > width - 1 && y > height - 1)
			throw std::range_error("Range error");
		return buffer[x][y];
	};
};

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,    25, 255, 255);
const TGAColor yellow= TGAColor(255, 255, 0,   255);
const TGAColor forest_green = TGAColor(34, 139, 34, 255);
const TGAColor aqua_marine = TGAColor(127, 255, 212, 255);
const TGAColor indigo = TGAColor(75,0,130, 255);
const TGAColor slate_gray = TGAColor(112,128,144, 255);

void drawLineClassic(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

void fillTriangel(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, TGAColor &color);
void fillTriangel(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, const TGAColor &color);

void fillTriangel(Vec3i a, Vec3i b, Vec3i c, TGAImage& image, TGAColor &color, Zbuffer &z_buffer);
void fillTriangel(Vec3i a, Vec3i b, Vec3i c, TGAImage& image, const TGAColor &color, Zbuffer &z_buffer);

uint32_t getDepth(Vec3i a, Vec3i b, Vec3i c, Vec2i m);
uint32_t getDepth(Vec3i a, Vec3i b, Vec2i m);

bool isInsideTriangel(Vec2i &a, Vec2i &b, Vec2i &c, Vec2i &point);
float toRadian(float degrees);

void fillTriangelInterpolation(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, TGAColor &color);
void fillTriangelInterpolation(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, const TGAColor &color);

void fillTriangelInterpolation(Vec3i v0, Vec3i v1, Vec3i v2, TGAImage& image, TGAColor &color, Zbuffer &z_buffer);
void fillTriangelInterpolation(Vec3i v0, Vec3i v1, Vec3i v2, TGAImage& image, const TGAColor &color, Zbuffer &z_buffer);

void fillRGB(Vec2i a, Vec2i b, Vec2i c, TGAImage& image);
Vec3f getBarycentric(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i point);

float bilineatInterpolation(Vec3f A, Vec3f B, Vec3f C, Vec3f D, Vec3f P);

void drawAxis(TGAImage &image);

void fillImage(TGAImage &image, TGAColor color);

void drawLineVertical(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color);
void drawLineHorizontal(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color);

void drawGrid(TGAImage& image, int step = 20, TGAColor color = red);
#endif