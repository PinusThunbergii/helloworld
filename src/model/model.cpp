#include "model.h"

Model::Model(const char *file) : vertices(), faces()
{
    this->OpenFile(file);
}

Model::~Model()
{
    if (fileReader.is_open())
        fileReader.close();
}

bool Model::OpenFile(const char *file)
{
    fileReader.open(file, std::fstream::in);
    bool result = false;
    if (fileReader.is_open())
        result = true;
    else
        return result;

    std::string line;
    char trash;
    while (!fileReader.eof())
    {
        std::getline(fileReader, line);
        std::stringstream temp(line);
        if (line.compare(0, 2, "v ") == 0)
        {
            temp >> trash;
            Vec3f item;

            temp >> item.x;
            temp >> item.y;
            temp >> item.z;
            vertices.push_back(item);
            //std::cout << item << std::endl;
            //std::cout << "Point {" << v[0] << ", " << v[1] << ", " << v[2] << "}" << std::endl;
        }
        else if (line.compare(0, 2, "f ") == 0)
        {
            //temp >> trash;
            std::vector<int> vec;
			std::vector<int> vec_t;
			std::vector<int> vec_n;
            std::stringstream test(line);
            std::string from_stream;

            test >> from_stream;
            while (!test.eof())
            {
                from_stream.erase();
                test >> from_stream;
                int i = 0;
                std::stringstream numeric_stream(from_stream);
                numeric_stream >> i;
                i--;
				numeric_stream >> trash;
				int t = 0;
				numeric_stream >> t;
				t--;
				numeric_stream >> trash;
				int n = 0;
				numeric_stream >> n;
				n--;
                vec.push_back(i);
				vec_t.push_back(t);
				vec_n.push_back(n);
            }
            faces.push_back(vec);
			faces_texture.push_back(vec_t);
			faces_normals.push_back(vec_n);
        }
		else if (line.compare(0, 3, "vt ") == 0)
		{
			std::cout << line << std::endl;
		}
    }

    fileReader.close();
    return result;
}


Vec3f Model::GetMax() const
{
	if (vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
	Vec3f max;
	max.x = vertices[0].x;
	max.y = vertices[0].y;
	max.z = vertices[0].z;
	for (auto iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		if (iter->x > max.x)
			max.x = iter->x;
		else if (iter->y > max.y)
			max.y = iter->y;
		else if (iter->z > max.z)
			max.z = iter->z;
	}
	return max;
}

Vec3f Model::GetMin() const
{
	if (vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
	Vec3f min;
	min.x = vertices[0].x;
	min.y = vertices[0].y;
	min.z = vertices[0].z;
	for (auto iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		if (iter->x < min.x)
			min.x = iter->x;
		else if (iter->y < min.y)
			min.y = iter->y;
		else if (iter->z < min.z)
			min.z = iter->z;
	}
	return min;
}

Vec3f Model::GetVertex(size_t i) const
{
	if (vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
	return vertices[i];
}

bool Model::LoadDiffuse(const char * file)
{
	if (diffuse.read_tga_file(file))
		hasDiffuse = true;
	return false;
}
