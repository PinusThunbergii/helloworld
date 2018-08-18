#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <limits>
#include <algorithm>

#include "tgaimage.h"
#include "geometry.h"
#include "drawing.h"
#include "model.h"
using std::cout;
using std::cin;
using std::endl;
using std::swap;

#define TEST false
#define RENDER false

void render(TGAImage &image, Model &model)
{
	model.RotateY(toRadian(15.0f));
	//model.RotateZ(toRadian(70.0f));
	model.RotateX(toRadian(30.0f));
	for(size_t x = 0; x < static_cast<size_t>(image.get_width()); x++)
		for(size_t y = 0; y < static_cast<size_t>(image.get_height()); y++)
			image.set(x, y, green);
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
	//Model model("/home/alex/c_cpp/graphic/obj/obj/cube.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/obj/untitled.obj");
	Model model("/home/alex/c_cpp/graphic/obj/african_head.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/obj/spider.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/obj/teapot.obj");
	render(image, model);
	image.flip_vertically(); 
	image.write_tga_file("/home/alex/c_cpp/graphic/img/output.tga");
	return 0;
}