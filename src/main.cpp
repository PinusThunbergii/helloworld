#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "tgaimage.h"
#include "geometry.h"
#include "drawing.h"
#include "model.h"
#include "interpolation.h"
#include "color.h"

using std::cout;
using std::cin;
using std::endl;
using std::swap;

#define TEST false
#define RENDER false

const float epsilon = 1e-1;

void render(TGAImage &image, Model &model)
{
	//model.RotateY(toRadian(15.0f));
	//model.RotateZ(toRadian(70.0f));
	//model.RotateX(toRadian(30.0f));
	fillImage(image, green);
	Vec2i center;
	uint width = image.get_width() - 1;
	uint height =  image.get_height() - 1;
	center.x = width / 2;
	center.y = height / 2;
	Vec3f max = model.GetMax();
	Vec3f min = model.GetMin();

	Vec3f delta_max_min = max - min;

	float k_x = width / delta_max_min.x;
	float k_y = height / delta_max_min.y;
	float min_k = k_x < k_y ? k_x : k_y;
	//float z_mult = (static_cast<float>(std::numeric_limits<uint32_t>::max()) / max.z) ;// * 0.133; //* 0.133
	//min_k *= 0.5;
	//std::numeric_limits<uint32_t>::max() / 2.0 
	float z_mult = (256.0f - 1.0f) / (max.z - min.z) ;//(( max.z - min.z) *255.0) ;

	Vec3f delta_max_min_screen = delta_max_min * min_k;
	float shift_x = center.x - delta_max_min_screen.x / 2.0f -1.0; //+ center.x / 2.0;
	float shift_y = center.y - delta_max_min_screen.y / 2.0f -1.0; //+ center.y / 2.0;

	std::mt19937 generator(15151);
	std::uniform_int_distribution<> uid(0, 255); 
	Zbuffer zbuffer(width+1, height+1);

	Vec3f eye_dir(0,0,-1);
	for (size_t i=0; i<model.FacesSize(); i++) 
	{
        std::vector<uint> face = model.GetFace(i);
		Vec3f world_coord[3];
		for(size_t i = 0; i < 3; i++)
			world_coord[i] = model.GetVertex(face[i]);
		Vec3i screen_coord[3];
		for(size_t i = 0; i < 3; i++)
			screen_coord[i] = Vec3i(fabs((world_coord[i].x - min.x) * min_k + shift_x), 
		        				    fabs((world_coord[i].y - min.y) * min_k + shift_y),
								    fabs((world_coord[i].z - min.z) * z_mult));

		//Vec3f n = cross(screen_coord[0] - screen_coord[1], screen_coord[0] - screen_coord[2]);
        //n.normalize();
        //float intensity = n*eye_dir;

		Vec3f n = cross((world_coord[2]-world_coord[0]),(world_coord[1] - world_coord[0]));
        n.normalize();
		float intensity = n*eye_dir;
		TGAColor color = TGAColor(intensity*255, intensity*255, intensity*255, 255);
		//TGAColor color = TGAColor(uid(generator), uid(generator), uid(generator), 255);
        if (intensity>0)
		{
			/*fillTriangel(screen_coord[0], screen_coord[1], screen_coord[2], 
						image, color, zbuffer);*/
						
			fillTriangelInterpolation(screen_coord[0], screen_coord[1], screen_coord[2], 
						image, color, zbuffer);
			//fillTriangel(Vec2i(a.x, a.y), Vec2i(b.x, b.y), Vec2i(c.x, c.y), image, color);
           	//fillTriangel(a, b, c, image, color, zbuffer);
		   	//fillTriangel(screen_coord[0], screen_coord[1], screen_coord[2],
			//   			image, color, zbuffer);
			//triangle(screen_coord[0], screen_coord[1], screen_coord[2],
			//			image, color, zbuffer);
        }
    }

	TGAImage z_buffer_image(zbuffer.width, zbuffer.height, TGAImage::GRAYSCALE);
	for(size_t x = 0; x < zbuffer.width; x++)
		for(size_t y = 0; y < zbuffer.height; y++)
			z_buffer_image.set(x, y, TGAColor(zbuffer(x,y), 1));
	z_buffer_image.flip_vertically();
	z_buffer_image.write_tga_file("/home/alex/c_cpp/graphic/img/z_buffer.tga");
}


int main(int argc, char** argv) {
	TGAImage image(1000, 1000, TGAImage::RGB);
	//drawGrid(image);
	
#if TEST == true
	std::mt19937 generator(15151);
	std::uniform_int_distribution<> uid(0, image.get_height()); 
	for(int i = 0; i < 250; i++)
		drawLine(uid(generator), uid(generator), uid(generator), uid(generator), image, white);
	for(int i = 0; i < 250; i++)
		//fillRGB(Vec2i(uid(generator), uid(generator)), Vec2i(uid(generator), uid(generator)), Vec2i(uid(generator), uid(generator)), image);
		//fillTriangelInterpolation(Vec2i(uid(generator), uid(generator)), Vec2i(uid(generator), uid(generator)), Vec2i(uid(generator), uid(generator)), image, green);
	
#endif
	//Model model("/home/alex/c_cpp/graphic/obj/cube.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/untitled.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/african_head.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/spider.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/teapot.obj");
	//render(image, model);
	//drawLineHorizontal(Vec2i(200, 200), Vec2i(500, 200), image, blue);
	//drawLineVertical(Vec2i(200, 200), Vec2i(200, 500), image, red);
	//drawGrid(image, 200);
	/*Vec2i v0(0, 0), v1(image.get_width() - 1, 255);
	
	for(int x = v0.x; x < v1.x; x++)
	{
		int p = v0.y + (x - v0.x) * int(v1.y - v0.y) / int(v1.x - v0.x);
		TGAColor colorR(p, 0, 0, 255);
		TGAColor colorG(0, p, 0, 255);
		TGAColor colorB(0, 0, p, 255);
		drawLineVertical(Vec2i(x, 0), Vec2i(x, 100), image, colorR);
		drawLineVertical(Vec2i(x, 100), Vec2i(x, 200), image, colorG);
		drawLineVertical(Vec2i(x, 200), Vec2i(x, 300), image, colorB);
	}*/
	/*Vec2i v00(0, 0), v01(0 , 1000), v11(1000, 1000), v10(1000, 0);
	Vec3i p00(255, 0, 0), p01(20, 0, 0), p11(255, 0, 0), p10(20, 0, 0);
	Vec2i p;
	for(p.x = v00.x; p.x < v11.x; p.x++)
		for(p.y = v00.y; p.y < v11.y; p.y++)
		{
			Vec3i vec_color  = bilinearInterpolation<Vec3i, Vec2i>(p00, p01, p11, p10, v00, v01, v11, v10, p);
			TGAColor color(vec_color.x, vec_color.y, vec_color.z, 255);
			image.set(p.x, p.y, color);
		}
	*/
	//Vec2i v0(0, 0), v1(0 , 1000), v2(1000, 1000), v3(1000, 0);
	int v0 = 100, v1 = 200, v2 = 400, v3 = 500;
	//Vec3i p0(255, 0, 0), p1(20, 0, 0), p2(255, 0, 0), p3(20, 0, 0);
	Vec3f p1[5];
	p1[0] = RGB2HSV(Vec3f(255, 0, 0));
	p1[1] = RGB2HSV(Vec3f(20, 153, 0));
	p1[2] = RGB2HSV(Vec3f(255, 255, 0));
	p1[3] = RGB2HSV(Vec3f(75, 130, 0));
	p1[4] = RGB2HSV(Vec3f(127, 255, 212));

	float p[5] = {100.0f, 300.0f, 150.0f, 250.0f, 400.0f};
	float v[5] = {100.0f, 200.0f, 300.0f, 400.0f, 500.0f};
	for(float x = v[0]; x < v[2]; x += epsilon)
	{
		float y = cubicInterpolation(p, v, x);
		Vec3f gg = HSV2RGB(cubicInterpolation(p1, v, x));
		TGAColor color(gg[0] , gg[1], gg[2], 255);
		image.set(x, y, color);
		drawLineVertical(Vec2i(x, 0), Vec2i(x, 75), image, color);
		image.set(x+500, gg[0]+100, red);
		image.set(x+500, gg[1]+100, green);
		image.set(x+500, gg[2]+100, blue);
	}
	for(float x = v[2]; x < v[4]; x += epsilon)
	{
		float y = cubicInterpolation(p+1, v+1, x);
		Vec3f gg = HSV2RGB(cubicInterpolation(p1+1, v+1, x));
		TGAColor color(gg[0] , gg[1], gg[2], 255);
		image.set(x, y, color);
		drawLineVertical(Vec2i(x, 0), Vec2i(x, 75), image, color);
		image.set(x+500, gg[0]+100, red);
		image.set(x+500, gg[1]+100, green);
		image.set(x+500, gg[2]+100, blue);
	}
	
	for(int i = 0; i < 5; i++)
	{
		Vec3f p2 = HSV2RGB(p1[i]);
		drawCircle(Vec2i(v[i], p[i]), 6, image, TGAColor(p2[0], p2[1], p2[2], 255));
	}	
	for(float x = v[0]; x < v[1]; x += epsilon)
	{
		float y = linearInterpolation<float, float>(p[0], p[1], v[0], v[1], x);
		Vec3f gg = HSV2RGB(linearInterpolation<Vec3f, float>(p1[0], p1[1], v[0], v[1], x));
		TGAColor color(int(gg[0]) % 255, int(gg[1])% 255, int(gg[2])% 255, 255);
		image.set(x, y, color);
		//cout << gg << endl;
		drawLineVertical(Vec2i(x, 450), Vec2i(x, 550), image, color);
		//cout << "( " << x << " ," << y << " )" << endl;
	}
	for(float x = v[1]; x < v[2]; x += epsilon)
	{
		float y = linearInterpolation<float, float>(p[1], p[2], v[1], v[2], x);
		Vec3f gg = HSV2RGB(linearInterpolation<Vec3f, float>(p1[1], p1[2], v[1], v[2], x));
		TGAColor color(int(gg[0]) % 255, int(gg[1])% 255, int(gg[2])% 255, 255);
		image.set(x, y, color);
		//cout << gg << endl;
		drawLineVertical(Vec2i(x, 450), Vec2i(x, 550), image, color);
		//cout << "( " << x << " ," << y << " )" << endl;
	}
	for(float x = v[2]; x < v[3]; x += epsilon)
	{
		float y = linearInterpolation<float, float>(p[2], p[3], v[2], v[3], x);
		Vec3f gg = HSV2RGB(linearInterpolation<Vec3f, float>(p1[2], p1[3], v[2], v[3], x));
		TGAColor color(int(gg[0]) % 255, int(gg[1])% 255, int(gg[2])% 255, 255);
		image.set(x, y, color);
		//cout << gg << endl;
		drawLineVertical(Vec2i(x, 450), Vec2i(x, 550), image, color);
		//cout << "( " << x << " ," << y << " )" << endl;
	}
	for(int i = 0; i < 360; i++)
	{
		RGB RgbColor = HSV2RGB(HSV(static_cast<float>(i), 1.0f, 1.0f));
		drawLineVertical(Vec2i(i, 750), Vec2i(i, 850), image, TGAColor(RgbColor.R, RgbColor.G, RgbColor.B, 255));
	}

	//std::cin.get();
	image.flip_vertically(); 
	image.write_tga_file("/home/alex/c_cpp/graphic/img/output.tga");
	return 0;
}