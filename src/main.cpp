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

#ifdef WINDOWS_BUILD
	#define STATIC_GETOPT
	#include "getopt.h"
#else
	#include <unistd.h>
	#include <getopt.h>
#endif 

#include "tgaimage.h"
#include "geometry.h"
#include "drawing.h"
#include "model.h"

using std::cout;
using std::cin;
using std::endl;
using std::swap;

Vec4f vec3f_to_vec4f_v(Vec3f v)
{
	Vec4f r;
	for (int i = 0; i < 3; i++)
		r[i] = v[i];
	r[3] = 0.0f;
	return r;
}

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
    m[2][2] = -depth/2.f;
    return m;
}

mat<4, 4, float> perspective_projection(float near, float far, float left, float right, float bottom, float top) 
{
	mat<4, 4, float> m;// = mat<4, 4, float>::identity();
	m[0][0] = 2 * near / (right - left);
    m[1][1] = 2 * near / (top - bottom);
	m[2][2] = -(far + near) / (far - near);
	m[3][2] = -1.0f;

	m[0][2] = (right + left) / (right - left);
    m[1][2] = (top + bottom) / (top - bottom);
	m[2][3] = -(2 * far * near) / (far - near);
    return m;
}

mat<4, 4, float> perspective_projection_fov(float near, float far, float fov_h, float fov_v)
{
	float top = tanf(toRadian(fov_v / 2.0f)) * fabs(near);
	float right = tanf(toRadian(fov_h / 2.0f)) * fabs(near);
	float bottom = -top;
	float left = -right;
	return perspective_projection(near, far, left, right, bottom, top); 
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

mat<4, 4, float> RotateZ(float a)
{
	float sin_a = sinf(toRadian(a));
	float cos_a = cosf(toRadian(a));

	mat<4, 4, float> m;
	m[1][1] = 1.0f;
	m[0][0] = cos_a;
	m[0][1] = -sin_a;
	m[1][0] = sin_a;
	m[1][1] = cos_a;
	m[3][3] = 1.0f;
	return m;
}

mat<4, 4, float> lookAt(Vec3f camera_pos, Vec3f up, Vec3f center)
{
	Vec3f z = (camera_pos - center).normalize();
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

void fill_interpolation(Vec3f *vertices, Vec3f *normals, Vec3f l, TGAImage& image, float **zbuffer)
{
	size_t width = image.get_width();
	size_t height = image.get_width();
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 2; j++)
		{
			if (vertices[i].y > vertices[j + 1].y)
			{
				std::swap(vertices[0], vertices[1]);
				std::swap(normals[0], normals[1]);
			}
		}
	}
	for (size_t i = 0; i < 3; i++)
	{
		std::cout << vertices[i] << std::endl;
	}
	
}


void fill(Vec3f *vertices, Vec2f *textures, Vec3f *normals, Vec3f l, Vec3f w_comp, TGAImage& image, TGAImage& texture, float **zbuffer)
{
	size_t width = image.get_width();
	size_t height = image.get_width();

	if ((vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x) || (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y))
		return;
	Vec2f top_left;
	top_left.x = vertices[0].x < vertices[1].x ? (vertices[0].x < vertices[2].x ? vertices[0].x : vertices[2].x) : (vertices[1].x < vertices[2].x ? vertices[1].x : vertices[2].x) + 0.5f;
	top_left.y = vertices[0].y > vertices[1].y ? (vertices[0].y > vertices[2].y ? vertices[0].y : vertices[2].y) : (vertices[1].y > vertices[2].y ? vertices[1].y : vertices[2].y) + 0.5f;
	Vec2f bottom_right;
	bottom_right.x = vertices[0].x > vertices[1].x ? (vertices[0].x > vertices[2].x ? vertices[0].x : vertices[2].x) : (vertices[1].x > vertices[2].x ? vertices[1].x : vertices[2].x) + 0.5f;
	bottom_right.y = vertices[0].y < vertices[1].y ? (vertices[0].y < vertices[2].y ? vertices[0].y : vertices[2].y) : (vertices[1].y < vertices[2].y ? vertices[1].y : vertices[2].y) + 0.5f;

	Vec2i current_point;
	Vec2i a_2(vertices[0].x + 0.5f, vertices[0].y + 0.5f), b_2(vertices[1].x + 0.5f, vertices[1].y + 0.5f), c_2(vertices[2].x + 0.5f, vertices[2].y + 0.5f);

//#define USE_NORMALS

	float light_intensity[3] = { l * normals[0], l * normals[1], l * normals[2] };
	
	//Vec3i n = cross(a-b, a-c);
	for (int x = top_left.x; x < bottom_right.x; x++)
	{
		if (x >= image.get_width())
			break;
		if (x < 0) continue;
		for (int y = bottom_right.y; y < top_left.y; y++)
		{
			current_point.x = x;
			current_point.y = y;
			if (isInsideTriangel(a_2, b_2, c_2, current_point))
			{
				int z = 0;
				Vec3f bc_coord = getBarycentric(a_2, b_2, c_2, current_point);
				z = (vertices[0].z * bc_coord[0] + vertices[1].z * bc_coord[1] + vertices[2].z * bc_coord[2]) + 0.5f;

				
				Vec3f bc_clip = Vec3f(bc_coord.x / w_comp[0], bc_coord.y / w_comp[1], bc_coord.z / w_comp[2]);
				bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

				float rPz_plus1 = 1.0f / (bc_coord[0] / w_comp[0] + bc_coord[1] / w_comp[1] + bc_coord[2] / w_comp[2]);
				bc_clip = Vec3f(bc_coord[0] / w_comp[0], bc_coord[1] / w_comp[1], bc_coord[2] / w_comp[2]) * rPz_plus1;
				//float z = (vertices[0].z * bc_clip[0] + vertices[1].z * bc_clip[1] + vertices[2].z * bc_clip[2]) + 0.5f;
				//float i = light_intensity[0] * bc_coord[0] + light_intensity[1] * bc_coord[1] + light_intensity[2] * bc_coord[2];
#ifdef USE_NORMALS
				Vec3f ad = normals[0] * bc_coord[0] + normals[1] * bc_coord[1] + normals[2] * bc_coord[2];
				Vec3f true_ad = normals[0] * bc_clip[0] + normals[1] * bc_clip[1] + normals[2] * bc_clip[2];
				float i = ad.normalize() * l;
				//drawLine(x, y, ad.x + x, ad.y + y, image, blue);
#else
				float i = light_intensity[0] * bc_coord[0] + light_intensity[1] * bc_coord[1] + light_intensity[2] * bc_coord[2];
				float true_i = light_intensity[0] * bc_clip[0] + light_intensity[1] * bc_clip[1] + light_intensity[2] * bc_clip[2];
#endif

				i = fabs(i);
				//std::cout << ad << " " << i << std::endl ;
				Vec2f diffuse_coord = textures[0] * bc_clip[0] + textures[1] * bc_clip[1] + textures[2] * bc_clip[2];
				//diffuse_coord.x = 1.0f - diffuse_coord.x;
				diffuse_coord.x *= texture.get_width();
				diffuse_coord.y = 1.0f - diffuse_coord.y;
				diffuse_coord.y *= texture.get_height();
				TGAColor diffuse_color = texture.get(diffuse_coord.x, diffuse_coord.y);
				if (y >= image.get_height() || y < 0)
					continue;
				if (zbuffer[x][y] < z)
				{
					zbuffer[x][y] = z;
					//TGAColor af(i * 255, i * 255, i * 255, 255);
					//TGAColor af(i * 255, i * 255, i * 255, 255);
					diffuse_color.r *= i;
					diffuse_color.g *= i;
					diffuse_color.b *= i;
					/*diffuse_color.r = bc_coord[0] * 255;
					diffuse_color.g = bc_coord[1] * 255;
					diffuse_color.b = bc_coord[2] * 255;*/
					image.set(x, y, diffuse_color);
					//std::cout << x << " | " << y << " | " << z << " | " << bc_coord << " || " << a.z << " " << b.z  << " "  << c.z  << std::endl;
				}
			}

		}
	}
	
}

void draw_edges(Vec3f *vertices, TGAImage& image)
{
	drawLine(vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, image, red);
	drawLine(vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, image, red);
	drawLine(vertices[2].x, vertices[2].y, vertices[0].x, vertices[0].y, image, red);
}

void draw_normals(Vec3f *vertices, Vec3f *normals, TGAImage& image)
{
	for(size_t i = 0; i < 3; i++)
		drawLine(vertices[i].x, vertices[i].y, (vertices[i].x + normals[i].x) * 1.05f, (vertices[i].y + normals[i].y) * 1.05f, image, blue);

}

template<class T>
T map(T input, T input_start, T input_end, T output_start, T output_end)
{
	//f(x) = (x - input_start) / (input_end - input_start) * (output_end - output_start) + output_start
	T output = (input - input_start) / (input_end - input_start) * (output_end - output_start) + output_start;
	return output;
}

void render(TGAImage &image, Model &model, std::string & zbuffer_name)
{
	float** z_buffer = new float*[image.get_width()];
	for (size_t i = 0; i < image.get_height(); i++)
		z_buffer[i] = new float[image.get_height()];
	
	for (size_t i = 0; i < image.get_width(); i++)
		for (size_t j = 0; j < image.get_height(); j++)
			z_buffer[i][j] = std::numeric_limits<float>::min();
	fillImage(image, green);
	Zbuffer zbuffer(image.get_width(), image.get_height());
	Vec3f camera_pos(0.0f, 0.0f, 2.5f);
	Vec3f center(0.0f, 0.0f, 0.0f);
	Vec3f up(0.0f, 1.0f, 0.0f);
	Vec3f light_dir(0.0f, 0.0f, -1.0f );

	//camera_pos =  vec4f_to_vec3f(RotateY(45.0f) * RotateX(15.0f) * vec3f_to_vec4f(camera_pos));
	camera_pos =  vec4f_to_vec3f(RotateX(-35.0f) * vec3f_to_vec4f(camera_pos));
	mat<4, 4, float> pro = perspective_projection_fov(1.0f, 8.0f, 60, 60);
	mat<4, 4, float> look = lookAt(camera_pos, up, center);
	mat<4, 4, float> viewportMatrix =  viewport(0, 0, image.get_width(), image.get_height(), 1024)  * pro * look;// * norm;* RotateX(35.0f) * RotateY(20.0f)  * pro

	for (size_t i=0; i<model.FacesSize(); i++) 
	{
		const std::vector<int>& face = model.GetFace(i);
		const std::vector<int>& texture = model.GetFaceTexture(i);
		const std::vector<int>& normal = model.GetFaceNormal(i);


		Vec3f model_space_vertices[3];
		Vec3f model_space_normal[3];
		Vec2f model_space_texture[3];
		for (size_t i = 0; i < 3; i++)
		{
			model_space_vertices[i] = model.GetVertex(face[i]);
			model_space_normal[i] = model.GetVertexNormal(normal[i]);
			model_space_texture[i] = model.GetVertexTexture(texture[i]);
		}
			
		
		Vec3f screen_space_vertices[3];
		Vec3f screen_space_normals[3];
		Vec3f w_comp;
		for(size_t i = 0; i < 3; i++)
		{
			//screen_space_vertices[i] = vec4f_to_vec3f(viewportMatrix * vec3f_to_vec4f(model_space_vertices[i]));
			Vec4f tmp = viewportMatrix * vec3f_to_vec4f(model_space_vertices[i]);
			screen_space_vertices[i] = vec4f_to_vec3f(tmp);
			w_comp[i] = tmp[3];
			//screen_space_normals[i] = vec4f_to_vec3f(viewportMatrix.transpose() * vec3f_to_vec4f_v(model_space_normal[i])).normalize();
			screen_space_normals[i] = vec4f_to_vec3f( look.invert().transpose()  * vec3f_to_vec4f_v(model_space_normal[i]) ).normalize();
		}							
		Vec3f n = cross((screen_space_vertices[2] - screen_space_vertices[0]),(screen_space_vertices[1] - screen_space_vertices[0])).normalize();
		float intensity = n*light_dir;
#define d 2
#if d
		TGAColor color = TGAColor(red.r*intensity, red.g*intensity, red.b*intensity, 255);
#else
		TGAColor color = red;
#endif
		if (intensity > 0.0f)
		{
#if d == 1
			fillTriangel(screen_space_vertices[0], screen_space_vertices[1], screen_space_vertices[2], image, color, zbuffer);
#elif d == 2
			fill(screen_space_vertices, model_space_texture, screen_space_normals, light_dir, w_comp, image, model.GetDiffuse(), z_buffer);
			//draw_edges(screen_space_vertices, image);
			//draw_normals(screen_space_vertices, screen_space_normals, image);
#else	
			//fillTriangel(screen_space_vertices[0], screen_space_vertices[1], screen_space_vertices[2], screen_space_normals[0], screen_space_normals[1], screen_space_normals[2], n, image, color, zbuffer);
#endif
        }
    }
	//zbuffer.write_image(zbuffer_name.c_str());
	TGAImage zb(image.get_width(), image.get_height(), TGAImage::RGB);
	for (size_t i = 0; i < image.get_width(); i++)
		for (size_t j = 0; j < image.get_height(); j++)
			zb.set(i, j, TGAColor(z_buffer[i][j] * 255.f, 0, 0, 255)); //map(z_buffer[i][j], std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), 0.f, 255.f)
	zb.flip_vertically();
	zb.write_tga_file(zbuffer_name.c_str());
	for (size_t i = 0; i < image.get_height(); i++)
		delete[] z_buffer[i];
	delete z_buffer;
}

int main(int argc, char** argv) 
{
	const char * options = "w:h:i:o:h:d:";
	int i = static_cast<char>('s');
	int width = 0, height = 0;
	std::string input, output, diffuse;
	
	do
	{
		switch (static_cast<char>(i))
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
		case 'd':
			diffuse = optarg;
			break;
		case 's':
			continue;
		case '?':
			std::cout << "Usage -w 1000 -h 1000 -i /home/user/example.obj -o /home/user/example.tga" << std::endl;
			return EXIT_FAILURE;
			break;
		default:
			return EXIT_FAILURE;
			break;
		}
	} while ((i = getopt(argc, argv, options)) != -1);

	std::string zbuffer_name;
	size_t extention_start;
	if((extention_start = output.find_last_of('.')) != std::string::npos)
		zbuffer_name = output.substr(0, extention_start);
	zbuffer_name += std::string("_z_buffer.tga");	
	TGAImage image(width, height, TGAImage::RGB);

	/*Model grid("..\\..\\obj\\plain.obj");
	grid.LoadDiffuse("..\\..\\obj\\grid_.tga");
	render(image, grid, zbuffer_name);*/

	Model model(input.c_str());
	model.LoadDiffuse(diffuse.c_str());
	render(image, model, zbuffer_name);
	image.flip_vertically(); 
	image.write_tga_file(output.c_str());
	//cin.get();
	return 0;
}