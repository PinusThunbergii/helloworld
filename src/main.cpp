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

mat<4, 4, float> viewport(int x, int y, int w, int h, int depth) 
{
    mat<4, 4, float> m;
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;
	m[3][3] = 1.0f;
    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
	//m[3][2] = 1.0f / -44.0f;
    return m;
}

mat<4, 4, float> projection(Vec3f near, Vec3f far, float fov) 
{
    mat<4, 4, float> m = mat<4, 4, float>::identity();
	float d = fabs(near.z) + 1.0f;
	float h = tanf(toRadian(fov / 2.0f)) *d;
	float k = 1.0f / h;
	
    m[0][0] = k;
    m[1][1] = k;
    //m[2][2] = -(far.z + near.z) / (far.z - near.z);
	m[2][2] = 1.0f;
	//m[0][3] = -h + 1.0f / h;
	//m[1][3] = -h + 1.0f / h;
	//m[2][3] = 1.0f;

   	//m[2][3] = -(2.0f * near.z * far.z) / (far.z - near.z);
	m[3][2] = -1.0f / d;
	//m[3][2] = -1.f/(Vec3f(1, 1, 3) -Vec3f(0, 0, 0)).norm();
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
	//m[3][3] = 1.0f / 0.5f;
    return m;
}

mat<4, 4, float> RotateX(float a)
{
	float sin_a = sinf(toRadian(a));
	float cos_a = cosf(toRadian(a));

	mat<4, 4, float> m;
	m[0][0] = 1.0f;
	m[1][1] = cos_a;
	m[1][2] = -sin_a;
	m[2][1] = sin_a;
	m[2][2] = cos_a;
	m[3][3] = 1.0f;
	return m;
}

mat<4, 4, float> RotateY(float a)
{
	float sin_a = sinf(toRadian(a));
	float cos_a = cosf(toRadian(a));

	mat<4, 4, float> m;
	m[1][1] = 1.0f;
	m[0][0] = cos_a;
	m[0][2] = -sin_a;
	m[2][0] = sin_a;
	m[2][2] = cos_a;
	m[3][3] = 1.0f;
	return m;
}

void render(TGAImage &image, Model &model)
{
	//model.RotateY(toRadian(30.0f));
	//model.RotateX(toRadian(30.0f));
	fillImage(image, green);
	Zbuffer zbuffer(image.get_width(), image.get_height());
	Vec3f eye_dir(0,0,-1);
	Vec3f bbox[2] = {model.GetMin(),  model.GetMax()};

	//mat<4, 4, float> viewportMatrix = viewport(500, 500, 400, 400, 255);
	//mat<4, 4, float> viewportMatrix =  viewport(0, 0, 400, 400, 255) * projection(Vec3f(0, 0, -7.0f), Vec3f(0,0,0), Vec3f(0, 0, -5.0f), Vec3f(0, 0, 5), 45.0f);// * normalized(bbox[0], bbox[1]);
	//viewportMatrix = viewportMatrix * projection(Vec3f(0, 0, -3.0f), Vec3f(0,0,0), Vec3f(0, 0, -1.0f), Vec3f(0, 0, 5), 90.0f);
	mat<4, 4, float> norm = normalized(bbox[0], bbox[1]);
	mat<4, 4, float> pro = projection(Vec3f(0, 0, -10.0f), Vec3f(0, 0, 8.0f), 50.0f);

	mat<4, 4, float> viewportMatrix =  viewport(200, 200, 600, 600, 255)  * pro * RotateX(35.0f) * RotateY(20.0f) ;//  * norm
	for (size_t i=0; i<model.FacesSize(); i++) 
	{
        std::vector<int> face = model.GetFace(i);

		Vec3f world_coord[3];
		for(size_t i = 0; i < 3; i++)
			world_coord[i] = model.GetVertex(face[i]);
		
		Vec3f screen_coord[3];
		for(size_t i = 0; i < 3; i++)
		{
			world_coord[i] = vec4f_to_vec3f(viewportMatrix * vec3f_to_vec4f(world_coord[i]));
			screen_coord[i] = world_coord[i] ;
			//screen_coord[i] = vec4f_to_vec3f(viewportMatrix * vec3f_to_vec4f(world_coord[i]));
			//cout << screen_coord[i] << " ";
		}							
		//cout << endl;

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
	
	//cout << bbox[0] << " " << bbox[1] << endl;
	//mat<4, 4, float> so = normalized(bbox[0], bbox[1]);
	//cout << so << endl;
	zbuffer.write_image("/home/alex/c_cpp/graphic/img/z_buffer.tga");
}

int main(int argc, char** argv) 
{
	TGAImage image(1000, 1000, TGAImage::RGB);
	//Model model("/home/alex/c_cpp/graphic/obj/cube.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/untitled.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/african_head.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/Vercetti.obj");
	//Model model("/home/alex/c_cpp/graphic/obj/spider.obj");
	Model model("/home/alex/c_cpp/graphic/obj/teapot.obj");
	render(image, model);
	image.flip_vertically(); 
	image.write_tga_file("/home/alex/c_cpp/graphic/img/output.tga");
	//cin.get();
	return 0;
}