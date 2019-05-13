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
                int v = 0;
				int t = 0;
				int n = 0;
                std::stringstream numeric_stream(from_stream);
                numeric_stream >> v;
                v--;
				//numeric_stream >> trash;
				
				if (numeric_stream.str().find("//") != std::string::npos)
				{
					numeric_stream >> trash; numeric_stream >> trash;
					numeric_stream >> n; 
					n--;
				}
				else
				{
					numeric_stream >> trash;
					numeric_stream >> t;
					t--;
					numeric_stream >> trash;
					numeric_stream >> n;
					n--;
				}
				
				if(v > -1)
					vec.push_back(v);
				if(t > -1)
					vec_t.push_back(t);
				if(n > -1)
					vec_n.push_back(n);
            }
			if (vec.size())
				faces.push_back(std::move(vec));
			if(vec_t.size())
				faces_texture.push_back(std::move(vec_t));
			if (vec_n.size())
				faces_normals.push_back(std::move(vec_n));
        }
		else if (line.compare(0, 3, "vt ") == 0)
		{
			//std::cout << line << std::endl;
			//vt  0.213 0.478 0.000
			std::stringstream test(line);
			test >> trash;	test >> trash;
			Vec2f vt;
			test >> vt.x;
			test >> vt.y;
			texture_vertices.push_back(std::move(vt));
		}
		else if (line.compare(0, 3, "vn ") == 0)
		{
			//std::cout << line << std::endl;
			//vn  0.001 0.482 -0.876
			std::stringstream test(line);
			test >> trash;	test >> trash;
			Vec3f vn;
			test >> vn.x;
			test >> vn.y;
			test >> vn.z;
			normals.push_back(std::move(vn));
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

Vec2f Model::GetVertexTexture(size_t i) const
{
	return texture_vertices[i];
}

Vec3f Model::GetVertexNormal(size_t i) const
{
	return normals[i];
}

const std::vector<int>& Model::GetFace(size_t i) const
{ 
	//if(i < 0 || i > faces.size())
	return faces[i]; 
};

const std::vector<int>& Model::GetFaceTexture(size_t i) const
{ 
	return faces_texture[i]; 
};

const std::vector<int>& Model::GetFaceNormal(size_t i) const
{ 
	return faces_normals[i]; 
};

std::string & Model::GetStat()
{
	std::string response;
	response = "dummy";
	return response;
}

size_t Model::VerticesSize() const
{	
	return vertices.size();
};
size_t Model::FacesSize() const
{ 
	return faces.size(); 
};

bool Model::LoadDiffuse(const char * file)
{
	if (diffuse.read_tga_file(file))
		hasDiffuse = true;
	return false;
}

TGAImage & Model::GetDiffuse()
{
	return diffuse;
}
