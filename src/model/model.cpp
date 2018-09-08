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

            std::stringstream test(line);
            std::string from_stream;

            test >> from_stream;
            while (!test.eof())
            {
                from_stream.erase();
                test >> from_stream;
                uint i = 0;
                std::stringstream numeric_stream(from_stream);
                numeric_stream >> i;
                i--;
                vec.push_back(i);
            }

            faces.push_back(vec);
        }
    }

    fileReader.close();
    return result;
}