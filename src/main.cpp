#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

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

mat<4, 4, float> lookAt(Vec3f camera_pos, Vec3f up, Vec3f center)
{
	//Vec3f z = (center - camera_pos).normalize();
	//Vec3f y = (up).normalize();
	//Vec3f x = cross(z, y).normalize();
	Vec3f z = (center - camera_pos).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();

	mat<4, 4, float> m = mat<4, 4, float>::identity();
	mat<4, 4, float> g = mat<4, 4, float>::identity();
	for(int i = 0; i < 3; i++)
	{
		m[0][i] = x[i];
		m[1][i] = y[i];
		m[2][i] = z[i];
		g[i][3] = -camera_pos[i];
	}
	return m * g;
}

void render(TGAImage &image, Model &model, std::string & zbuffer_name)
{
	fillImage(image, green);
	Zbuffer zbuffer(image.get_width(), image.get_height());
	Vec3f camera_pos(1.0, 0.0, 1.0);
	Vec3f center(0.0, 0.0, 0.0);
	Vec3f up(0.0, 1.0, 0.0);
	mat<4, 4, float> pro = projection(Vec3f(0, 0, -2.0f), Vec3f(0, 0, 5.0f), 20.0f);//Vec3f(0, 0, -0.5f)
	pro = mat<4, 4, float>::identity();
	//pro[3][2] = -1.0f / 20.0f;
	mat<4, 4, float> look = lookAt(camera_pos, up, center);
	mat<4, 4, float> viewportMatrix =  viewport(200, 200, 600, 600, 255) * pro  * look ;// * norm;* RotateX(35.0f) * RotateY(20.0f)

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
		}							
		Vec3f n = cross((world_coord[2]-world_coord[0]),(world_coord[1] - world_coord[0]));

        n.normalize();
		float intensity = n*(center-camera_pos).normalize();// eye_dir
		TGAColor color = TGAColor(intensity*255, intensity*255, intensity*255, 255);
        //if (intensity != 0)
		if (intensity > 0)
		{
			fillTriangel(screen_coord[0], screen_coord[1], screen_coord[2], 
						image, color, zbuffer);
        }
    }
	zbuffer.write_image(zbuffer_name.c_str());
}

int main(int argc, char** argv) 
{
	const char * options = "w:h:i:o:h";
	int i = 0;
	int width = 0, height = 0;
	std::string input, output;
	while((i = getopt(argc, argv, options)) != -1)
	{
		switch(static_cast<char>(i))
		{
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
			case 'i':
				input = optarg;
				break;
			case 'o':
				output = optarg;
				break;
			case '?':
				std::cout << "Usage -w 1000 -h 1000 -i /home/user/example.obj -o /home/user/example.tga" << std::endl;
				return EXIT_FAILURE;
				break;	
			default:
				return EXIT_FAILURE;
				break;				
		}
	}
	std::string zbuffer_name;
	size_t extention_start;
	if((extention_start = output.find_last_of('.')) != std::string::npos)
		zbuffer_name = output.substr(0, extention_start);
	zbuffer_name += std::string("_z_buffer.tga");	
	TGAImage image(width, height, TGAImage::RGB);
	Model model(input.c_str());
	render(image, model, zbuffer_name);
	image.flip_vertically(); 
	image.write_tga_file(output.c_str());
	return 0;
}