#include <math.h>
#include "drawing.h"
using std::swap;

void drawLineClassic(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	int delta_x = abs(x1 - x0);
	int delta_y = abs(y1 - y0);
	bool isSwaped = false;
	if(delta_x < delta_y)
	{	
		isSwaped = true;
		swap(x1, y1);
		swap(x0, y0);
	}
	if(x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}
	float k = static_cast<float>(y1 - y0) / static_cast<float>(x1 - x0); 

	int y = 0;
	for(int x = x0; x <= x1; x++)
	{
		y = (x - x0) * k + y0;
		if(isSwaped) image.set(y, x, color);
		else 		 image.set(x, y, color);
	}
}

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	int delta_x = abs(x1 - x0);
	int delta_y = abs(y1 - y0);
	bool isSwaped = false;

	if(delta_x < delta_y)
	{	
		isSwaped = true;
		swap(x1, y1);
		swap(x0, y0);
	}
	if(x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}
	delta_x = abs(x1 - x0);
	delta_y = abs(y1 - y0);

	float D = delta_y * 2 - delta_x;
	int y_sing = y1 > y0 ? 1 : -1;
	int y = y0;

	for(int x = x0; x <= x1; x++)
	{
		if(isSwaped) image.set(y, x, color);
		else 		 image.set(x, y, color);

		if(D > 0)
		{
			y += y_sing;
			D -= 2 * delta_x;
		}
		D += 2 * delta_y;
	}
}

void fillTriangel(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, TGAColor &color)
{
	if((a.x == b.x && a.x == c.x) || (a.y == b.y && a.y == c.y))
		return;
	Vec2i top_left;
	top_left.x = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
	top_left.y = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);
	Vec2i bottom_right;
	bottom_right.x = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
	bottom_right.y = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);

	Vec2i current_point;
	for(int x = top_left.x; x < bottom_right.x; x++)
	{
		for(int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x; 
			current_point.y = y;
			if(isInsideTriangel(a, b, c, current_point))	
				image.set(x, y, color);
		}
	}
	#if DRAW_BOUNDIND_BOX == true
	drawLine(top_left.x, top_left.y, top_left.x, bottom_right.y, image, red);
	drawLine(top_left.x, top_left.y, bottom_right.x, top_left.y, image, red);
	drawLine(top_left.x, bottom_right.y, bottom_right.x, bottom_right.y, image, red);
	drawLine(bottom_right.x, top_left.y, bottom_right.x, bottom_right.y, image, red);
	#endif

	#if DRAW_EDGES == true
	drawLine(a.x, a.y, b.x, b.y, image, red);
	drawLine(b.x, b.y, c.x, c.y, image, red);
	drawLine(c.x, c.y, b.x, b.y, image, red);
	#endif
}

void fillTriangel(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, const TGAColor &color)
{
	if((a.x == b.x && a.x == c.x) || (a.y == b.y && a.y == c.y))
		return;
	Vec2i top_left;
	top_left.x = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
	top_left.y = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);
	Vec2i bottom_right;
	bottom_right.x = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
	bottom_right.y = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);

	Vec2i current_point;
	for(int x = top_left.x; x < bottom_right.x; x++)
	{
		for(int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x; 
			current_point.y = y;
			if(isInsideTriangel(a, b, c, current_point))	
				image.set(x, y, color);
		}
	}
	#if DRAW_BOUNDIND_BOX == true
	drawLine(top_left.x, top_left.y, top_left.x, bottom_right.y, image, red);
	drawLine(top_left.x, top_left.y, bottom_right.x, top_left.y, image, red);
	drawLine(top_left.x, bottom_right.y, bottom_right.x, bottom_right.y, image, red);
	drawLine(bottom_right.x, top_left.y, bottom_right.x, bottom_right.y, image, red);
	#endif

	#if DRAW_EDGES == true
	drawLine(a.x, a.y, b.x, b.y, image, red);
	drawLine(b.x, b.y, c.x, c.y, image, red);
	drawLine(c.x, c.y, b.x, b.y, image, red);
	#endif
}

void fillTriangel(Vec3i a, Vec3i b, Vec3i c, TGAImage& image, TGAColor &color, Zbuffer &z_buffer)
{
	if((a.x == b.x && a.x == c.x) || (a.y == b.y && a.y == c.y))
		return;
	Vec2i top_left;
	top_left.x = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
	top_left.y = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);
	Vec2i bottom_right;
	bottom_right.x = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
	bottom_right.y = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);

	Vec2i current_point;
	Vec2i a_2(a.x, a.y), b_2(b.x, b.y), c_2(c.x, c.y);
	//Vec3i n = cross(a-b, a-c);
	for(int x = top_left.x; x < bottom_right.x; x++)
	{
		for(int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x; 
			current_point.y = y;
			if(isInsideTriangel(a_2, b_2, c_2, current_point))
			{
				//uint32_t z = getDepth(a, b, c, current_point);
				//uint32_t z = getDepth(n, a, current_point);
				uint32_t z = 0;
				Vec3f bc_coord = getBarycentric(a_2, b_2, c_2, current_point);
				z = a.z * bc_coord[0] + b.z * bc_coord[1] + c.z * bc_coord[2];
				if(z_buffer(x, y) < z)
				{
					z_buffer(x, y) = z;
					image.set(x, y, color);
					//std::cout << x << " | " << y << " | " << z << std::endl;
				}
			}	
				
		}
	}
	#if DRAW_BOUNDIND_BOX == true
	drawLine(top_left.x, top_left.y, top_left.x, bottom_right.y, image, red);
	drawLine(top_left.x, top_left.y, bottom_right.x, top_left.y, image, red);
	drawLine(top_left.x, bottom_right.y, bottom_right.x, bottom_right.y, image, red);
	drawLine(bottom_right.x, top_left.y, bottom_right.x, bottom_right.y, image, red);
	#endif

	#if DRAW_EDGES == true
	drawLine(a.x, a.y, b.x, b.y, image, red);
	drawLine(b.x, b.y, c.x, c.y, image, red);
	drawLine(c.x, c.y, b.x, b.y, image, red);
	#endif
}

void fillTriangel(Vec3i a, Vec3i b, Vec3i c, TGAImage& image, const TGAColor &color, Zbuffer &z_buffer)
{
	if((a.x == b.x && a.x == c.x) || (a.y == b.y && a.y == c.y))
		return;
	Vec2i top_left;
	top_left.x = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
	top_left.y = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);
	Vec2i bottom_right;
	bottom_right.x = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
	bottom_right.y = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);


	Vec2i current_point;
	Vec2i a_2(a.x, a.y), b_2(b.x, b.y), c_2(c.x, c.y);
	for(int x = top_left.x; x < bottom_right.x; x++)
	{
		for(int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x; 
			current_point.y = y;
			if(isInsideTriangel(a_2, b_2, c_2, current_point))	
			{
				//uint32_t z = getDepth(n, a, current_point);
				uint32_t z = 0;
				Vec3f bc_coord = getBarycentric(a_2, b_2, c_2, current_point);
				z = a.z * bc_coord[0] + b.z * bc_coord[1] + c.z * bc_coord[2];
				if(z_buffer(x, y) < z)
				{
					z_buffer(x, y) = z;
					image.set(x, y, color);
					//std::cout << x << " | " << y << " | " << z << std::endl;
				}
			}	
		}
	}
	#if DRAW_BOUNDIND_BOX == true
	drawLine(top_left.x, top_left.y, top_left.x, bottom_right.y, image, red);
	drawLine(top_left.x, top_left.y, bottom_right.x, top_left.y, image, red);
	drawLine(top_left.x, bottom_right.y, bottom_right.x, bottom_right.y, image, red);
	drawLine(bottom_right.x, top_left.y, bottom_right.x, bottom_right.y, image, red);
	#endif

	#if DRAW_EDGES == true
	drawLine(a.x, a.y, b.x, b.y, image, red);
	drawLine(b.x, b.y, c.x, c.y, image, red);
	drawLine(c.x, c.y, b.x, b.y, image, red);
	#endif
}

bool isInsideTriangel(Vec2i &a, Vec2i &b, Vec2i &c, Vec2i &point)
{
	int f_ab = 0, f_bc = 0, f_ac = 0;

	f_ab = (b.y - a.y) * (point.x - a.x) - (b.x - a.x) * (point.y - a.y);
	f_bc = (c.y - b.y) * (point.x - b.x) - (c.x - b.x) * (point.y - b.y);
	f_ac = (a.y - c.y) * (point.x - c.x) - (a.x - c.x) * (point.y - c.y);
	if(f_ab >= 0 && f_bc >= 0 && f_ac >= 0)
		return true;
	else if(f_ab <= 0 && f_bc <= 0 && f_ac <= 0)
		return true;
	else 
		return false;
}

float toRadian(float degrees)
{
	return degrees * static_cast<float>(M_PI) / 180.0f;
}

uint32_t getDepth(Vec3i a, Vec3i b, Vec3i c, Vec2i m)
{
	Vec3i n = cross(a, b);
	//if(n.z == 0)
	//	return std::numeric_limits<uint32_t>::min(); //std::numeric_limits<uint32_t>::max();
	//uint32_t z = ( ( n.x * (m.x - c.x) + n.y * (m.y - c.y) ) / - n.z ) - c.z;
	uint32_t numerator = n.x * (m.x - c.x) + n.y * (m.y - c.y);
	uint32_t denominator = n.z;
	return (numerator / denominator) - c.z;
}

uint32_t getDepth(Vec3i n, Vec3i b, Vec2i m)
{
	return (-n.x * (m.x - b.x) - n.y * (m.y - b.y)) / n.z - b.z;
}

void fillRGB(Vec2i a, Vec2i b, Vec2i c, TGAImage& image)
{
	if((a.x == b.x && a.x == c.x) || (a.y == b.y && a.y == c.y))
		return;
	Vec2i top_left;
	top_left.x = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
	top_left.y = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);
	Vec2i bottom_right;
	bottom_right.x = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
	bottom_right.y = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);

	Vec2i current_point;
	for(int x = top_left.x; x < bottom_right.x; x++)
	{
		for(int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x; 
			current_point.y = y;
			Vec3f barycentric = getBarycentric(a, b, c, current_point);
			if(barycentric[0] < 1.0f && barycentric[0] > 0.0f 
			&& barycentric[1] < 1.0f && barycentric[1] > 0.0f
			&& barycentric[2] < 1.0f && barycentric[2] > 0.0f )	
				image.set(x, y, TGAColor(255.0f*barycentric[0], 255.0f*barycentric[1], 255.0f*barycentric[2], 255));
		}
	}
	#if DRAW_BOUNDIND_BOX == true
	drawLine(top_left.x, top_left.y, top_left.x, bottom_right.y, image, red);
	drawLine(top_left.x, top_left.y, bottom_right.x, top_left.y, image, red);
	drawLine(top_left.x, bottom_right.y, bottom_right.x, bottom_right.y, image, red);
	drawLine(bottom_right.x, top_left.y, bottom_right.x, bottom_right.y, image, red);
	#endif

	#if DRAW_EDGES == true
	drawLine(a.x, a.y, b.x, b.y, image, red);
	drawLine(b.x, b.y, c.x, c.y, image, red);
	drawLine(c.x, c.y, b.x, b.y, image, red);
	#endif
}

Vec3f getBarycentric(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i point)
{
	Vec3f barycentric;
	float det = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y  - v2.y);
	barycentric[0] = (v1.y - v2.y) * (point.x - v2.x) + (v2.x - v1.x) * (point.y - v2.y);
	barycentric[1] = (v2.y - v0.y) * (point.x - v2.x) + (v0.x - v2.x) * (point.y - v2.y);
	barycentric[0] /= det;
	barycentric[1] /= det;
	barycentric[2] = 1.0f - (barycentric[0] + barycentric[1]);
	return barycentric;
}

void fillTriangelInterpolation(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, TGAColor &color)
{
	if(v0.y < v1.y) swap(v0, v1);
	if(v0.y < v2.y) swap(v0, v2);
	if(v1.y < v2.y) swap(v1, v2);

	float alpha = static_cast<float>(v1.x - v0.x) / static_cast<float>(v1.y - v0.y);
	float beta = static_cast<float>(v2.x - v0.x) / static_cast<float>(v2.y - v0.y);
	float gamma = static_cast<float>(v2.x - v1.x) / static_cast<float>(v2.y - v1.y);

	int v0_v2_split = v0.x + (v1.y - v0.y) * beta;

	/*
	for(int x = v1.x; x < v0_v2_split; x++)
		image.set(x, v1.y, indigo);
	*/
	
	for(int y = v0.y; y > v1.y; y--)
	{
		int left_x = v0.x + (y - v0.y) * alpha;
		int right_x = v0.x + (y - v0.y) * beta;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
	for(int y = v1.y; y > v2.y; y--)
	{
		int left_x = v1.x + (y - v1.y) * gamma;
		int right_x = v0_v2_split + (y - v1.y) * beta;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
}

void fillTriangelInterpolation(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, const TGAColor &color)
{
	if(v0.y < v1.y) swap(v0, v1);
	if(v0.y < v2.y) swap(v0, v2);
	if(v1.y < v2.y) swap(v1, v2);

	float alpha = static_cast<float>(v1.x - v0.x) / static_cast<float>(v1.y - v0.y);
	float beta = static_cast<float>(v2.x - v0.x) / static_cast<float>(v2.y - v0.y);
	float gamma = static_cast<float>(v2.x - v1.x) / static_cast<float>(v2.y - v1.y);

	int v0_v2_split = v0.x + (v1.y - v0.y) * beta;

	/*
	for(int x = v1.x; x < v0_v2_split; x++)
		image.set(x, v1.y, indigo);
	*/
	
	for(int y = v0.y; y > v1.y; y--)
	{
		int left_x = v0.x + (y - v0.y) * alpha;
		int right_x = v0.x + (y - v0.y) * beta;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
	for(int y = v1.y; y > v2.y; y--)
	{
		int left_x = v1.x + (y - v1.y) * gamma;
		int right_x = v0_v2_split + (y - v1.y) * beta;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
}

void fillTriangelInterpolation(Vec3i v0, Vec3i v1, Vec3i v2, TGAImage& image, TGAColor &color, Zbuffer &z_buffer)
{
	if(v0.y < v1.y) swap(v0, v1);
	if(v0.y < v2.y) swap(v0, v2);
	if(v1.y < v2.y) swap(v1, v2);

	float alpha = static_cast<float>(v1.x - v0.x) / static_cast<float>(v1.y - v0.y);
	float beta = static_cast<float>(v2.x - v0.x) / static_cast<float>(v2.y - v0.y);
	float gamma = static_cast<float>(v2.x - v1.x) / static_cast<float>(v2.y - v1.y);

	int v0_v2_split = v0.x + (v1.y - v0.y) * beta;

	/*
	for(int x = v1.x; x < v0_v2_split; x++)
		image.set(x, v1.y, indigo);
	*/
	
	for(int y = v0.y; y > v1.y; y--)
	{
		int left_x = ((v0.x) + (y - v0.y) * alpha);
		int right_x = ((v0.x) + (y - v0.y) * beta);

		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
	for(int y = v1.y; y > v2.y; y--)
	{
		int left_x = (v1.x + (y - v1.y) * gamma);
		int right_x = (v0_v2_split + (y - v1.y) * beta);

		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
}

void fillTriangelInterpolation(Vec3i v0, Vec3i v1, Vec3i v2, TGAImage& image, const TGAColor &color, Zbuffer &z_buffer)
{
	if(v0.y < v1.y) swap(v0, v1);
	if(v0.y < v2.y) swap(v0, v2);
	if(v1.y < v2.y) swap(v1, v2);

	float alpha = static_cast<float>(v1.x - v0.x) / static_cast<float>(v1.y - v0.y);
	float beta = static_cast<float>(v2.x - v0.x) / static_cast<float>(v2.y - v0.y);
	float gamma = static_cast<float>(v2.x - v1.x) / static_cast<float>(v2.y - v1.y);

	int v0_v2_split = v0.x + (v1.y - v0.y) * beta;

	/*
	for(int x = v1.x; x < v0_v2_split; x++)
		image.set(x, v1.y, indigo);
	*/
	
	for(int y = v0.y; y > v1.y; y--)
	{
		int left_x = v0.x + (y - v0.y) * alpha;
		int right_x = v0.x + (y - v0.y) * beta ;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
	for(int y = v1.y; y > v2.y; y--)
	{
		int left_x = v1.x + (y - v1.y) * gamma;
		int right_x = v0_v2_split + (y - v1.y) * beta;
		if(left_x > right_x) swap(left_x, right_x);
		for(int x = left_x; x <= right_x; x++)
			image.set(x, y, color);
	}
}

void drawAxis(TGAImage &image)
{
	int width = image.get_width() - 1;
	int height =  image.get_height() - 1;
	Vec2i center;
	center.x = image.get_width() / 2;
	center.y = image.get_height() / 2;
	drawLine(center.x, center.y, 0, center.y, image, red);
	drawLine(center.x, center.y, center.x, 0, image, green);
	drawLine(center.x, center.y, 0, 0, image, blue);
	drawLine(center.x, center.y, 0, height, image, white);

	drawLine(center.x, center.y, width, center.y, image, red);
	drawLine(center.x, center.y, center.x, height, image, green);
	drawLine(center.x, center.y, width, height, image, blue);
	drawLine(center.x, center.y, width, 0, image, white);

	drawLine(center.x, center.y, width - 100, height, image, yellow);
	drawLine(center.x, center.y, width, height - 100, image, yellow);

	drawLine(center.x, center.y, 100, 0, image, yellow);
	drawLine(center.x, center.y, 0, 100, image, yellow);

	drawLine(center.x, center.y, width, 100, image, yellow);
	drawLine(center.x, center.y, width-100, 0, image, yellow);

	drawLine(center.x, center.y, 0, height -100, image, yellow);
	drawLine(center.x, center.y, 100, height, image, yellow);

}

float bilineatInterpolation(Vec3f A, Vec3f B, Vec3f C, Vec3f D, Vec3f P)
{
	float Zm = A.z + (P.x - A.x) * (D.z - A.z) / (D.x - A.x);
	float Zn = B.z + (P.x - B.x) * (C.z - B.z) / (C.x - B.x);
	float Zp = Zm + (P.y - A.y) * (Zn - Zm) / (B.y - A.y);
}

void fillImage(TGAImage &image, TGAColor color)
{
	for(size_t x = 0; x < static_cast<size_t>(image.get_width()); x++)
		for(size_t y = 0; y < static_cast<size_t>(image.get_height()); y++)
			image.set(x, y, color);
}

void drawLineHorizontal(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color)
{
	if(v0.y != v1.y) return;
	if(v1.x < v0.x) swap(v0, v1);
	for(int x = v0.x; x <= v1.x; x++)	
		image.set(x, v0.y, color);
}

void drawLineVertical(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color)
{
	if(v0.x != v1.x) return;
	if(v1.y < v0.y) swap(v0, v1);
	for(int y = v0.y; y <= v1.y; y++)
		image.set(v0.x, y, color);
}

void drawGrid(TGAImage& image, int step, TGAColor color)
{
	int height = image.get_height();
	int width = image.get_width();
	for(int y = 0;  y <= height; y+=step)
		drawLineHorizontal(Vec2i(0, y), Vec2i(width - 1, y), image, color);
	for(int x = 0;  x <= width; x+=step)
		drawLineVertical(Vec2i(x, 0), Vec2i(x, height - 1), image, color);
}