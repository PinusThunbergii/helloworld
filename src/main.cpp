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

Vec4f vec3f_to_vec4f(Vec3f v)
{
	Vec4f r;
	for(int i = 0; i < 3; i++)
		r[i] = v[i];
	r[3] = 1.0f;
	return r;	
}

Vec3f vec4f_to_vec3f(Vec4f v)
{
	return Vec3f(v[0]/v[3], v[1]/v[3], v[2]/v[3]);
}

mat<4, 4, float> ndc(float x0, float y0, float z0, float x1, float y1, float z1)
{
	mat<4, 4, float> m;
	m[0][0] = 2.0f / (x1 - x0);
	m[1][1] = 2.0f / (y1 - y0);
	m[2][2] = 2.0f / (z1 - z0);
	m[3][3] = 1.0f;

	m[0][3] = (x1 - x0) / 2.0f + 1.0f;
	m[1][3] = (y1 - y0) / 2.0f + 1.0f;
	m[2][3] = (z1 - z0) / 2.0f + 1.0f;
	return m;
}

mat<4, 4, float> toScreen(Vec3f v0, Vec3f v1, float dept, int w, int h)
{
	Vec3f delta = v0 - v1;
	float k = std::min(delta.x, delta.y);
	mat<4, 4, float> m;
	m[0][0] = w / k;
	m[1][1] = h / k;
	m[2][2] = dept/k;
	m[3][3] = 1.0f;
	float shift_x = w / 2.0f - delta.x / 2.0f; //+ center.x / 2.0;
	float shift_y = h / 2.0f - delta.y / 2.0f; 
	//m[0][3] = -v0[0] * w / k + w;
	//m[1][3] = -v0[1] * h / k + h;
	//m[2][3] = v0[2] * dept / k + dept;
	m[0][3] = -v0[0] * k + shift_x;
	m[1][3] = -v0[1] * k + shift_y;
	m[2][3] = -v0[2] * dept / k + dept;
	return m;
}


mat<4, 4, float> viewport(Vec3f delta, int w, int h)
{
	mat<4, 4, float> vp;
	float k = std::min(std::min(delta[0], delta[1]), delta[2]);
	vp[0][0] = w/k;// * w/2.0f;
	vp[1][1] = h/k;// * h/2.0f;
	vp[2][2] = 255.0f/k;
	vp[3][3] = 1.0f;
	vp[0][3] = w / 2.0f - delta.x / 2.0f;
	vp[1][3] = h / 2.0f - delta.y / 2.0f;
	vp[2][3] = 255.0f / 2.0f;
	return vp;
} 
mat<4, 4, float> viewport(int x, int y, int w, int h, int depth) {
    mat<4, 4, float> m;
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;
	m[3][3] = 1.0f;
    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
//	m[3][3] = -1.0f /5.0f;
    return m;
}

mat<4, 4, float> normalized(Vec3f min, Vec3f max, float min_n = -1.0f, float max_n = 1.0f) {
    mat<4, 4, float> m;
	Vec3f delta = max - min;
	float k = (max_n - min_n) / std::max(std::max(delta.x, delta.y), delta.z);
	m[0][0] = k;
    m[1][1] = k;
    m[2][2] = k;
	m[3][3] = 1.0f;

    m[0][3] = min_n - min[0] * k;
    m[1][3] = min_n - min[1] * k;
    m[2][3] = min_n - min[2] * k;

    return m;
}

void render(TGAImage &image, Model &model)
{
	//model.RotateX(toRadian(90.0f));
	fillImage(image, green);
	Zbuffer zbuffer(image.get_width(), image.get_height());
	Vec3f eye_dir(0,0,-1);
	Vec3f bbox[2] = {model.GetMin(),  model.GetMax()};

	//mat<4, 4, float> viewportMatrix = viewport(500, 500, 400, 400, 255);
	mat<4, 4, float> viewportMatrix =  viewport(500, 500, 500, 500, 255) * normalized(bbox[0], bbox[1]);
	
	for (size_t i=0; i<model.FacesSize(); i++) 
	{
        std::vector<uint> face = model.GetFace(i);

		Vec3f world_coord[3];
		for(size_t i = 0; i < 3; i++)
			world_coord[i] = model.GetVertex(face[i]);
		
		Vec3f screen_coord[3];
		for(size_t i = 0; i < 3; i++)
		{
			screen_coord[i] = vec4f_to_vec3f(viewportMatrix * vec3f_to_vec4f(world_coord[i]));
			cout << screen_coord[i] << " ";
		}							
		cout << endl;

		Vec3f n = cross((world_coord[2]-world_coord[0]),(world_coord[1] - world_coord[0]));
        n.normalize();
		float intensity = n*eye_dir;
		TGAColor color = TGAColor(intensity*255, intensity*255, intensity*255, 255);
        if (intensity>0)
		{
			fillTriangel(screen_coord[0], screen_coord[1], screen_coord[2], 
						image, color, zbuffer);
						
			/*fillTriangelInterpolation(screen_coord[0], screen_coord[1], screen_coord[2], 
						image, color, zbuffer);*/
			//fillTriangel(Vec2i(a.x, a.y), Vec2i(b.x, b.y), Vec2i(c.x, c.y), image, color);
           	//fillTriangel(a, b, c, image, color, zbuffer);
		   	//fillTriangel(screen_coord[0], screen_coord[1], screen_coord[2],
			//   			image, color, zbuffer);
			//triangle(screen_coord[0], screen_coord[1], screen_coord[2],
			//			image, color, zbuffer);
        }
    }
	
	cout << bbox[0] << " " << bbox[1] << endl;
	mat<4, 4, float> so = normalized(bbox[0], bbox[1]);
	cout << so << endl;
	zbuffer.write_image("/home/alex/c_cpp/graphic/img/z_buffer.tga");
}

int main(int argc, char** argv) {
	TGAImage image(1000, 1000, TGAImage::RGB);
	//Model model("/home/alex/c_cpp/graphic/obj/cube.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/untitled.obj");
	Model model("/home/alex/c_cpp/graphic/obj/african_head.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/Vercetti.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/spider.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/teapot.obj");
	render(image, model);
	image.flip_vertically(); 
	image.write_tga_file("/home/alex/c_cpp/graphic/img/output.tga");
	cin.get();
	return 0;
}