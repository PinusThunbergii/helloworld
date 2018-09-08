#ifndef _MODEL_
#define _MODEL_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "geometry.h"

class Model
{
public: 
    Model(const char * file);
    Model(){};
    ~Model();
    bool OpenFile(const char * file);
    size_t VerticesSize() const
    {return vertices.size();};

    size_t FacesSize() const
    {   return faces.size();};

    std::vector<int> GetFace(size_t i) const
    {   return faces[i];    };

    Vec3f GetMax() const
    {
        if(vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
        Vec3f max;
        max.x = vertices[0].x;
        max.y = vertices[0].y;
        max.z = vertices[0].z;
        for(auto iter = vertices.begin(); iter != vertices.end(); iter++)
        {
            if(iter->x > max.x)
                max.x = iter->x;
            else if(iter->y > max.y)
                max.y = iter->y;
            else if(iter->z > max.z)
                max.z = iter->z;
        }
        return max;
    };

    Vec3f GetMin() const
    {
        if(vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
        Vec3f min;
        min.x = vertices[0].x;
        min.y = vertices[0].y;
        min.z = vertices[0].z;
        for(auto iter = vertices.begin(); iter != vertices.end(); iter++)
        {
            if(iter->x < min.x)
                min.x = iter->x;
            else if(iter->y < min.y)
                min.y = iter->y;
            else if(iter->z < min.z)
                min.z = iter->z;
        }
        return min;
    };

    Vec3f GetVertex(size_t i) const
    {   
        if(vertices.empty()) return Vec3f(-1.0f, -1.0f, -1.0f);
        return vertices[i];
    };

    void RotateZ(float a)
    {
        for(auto iter = vertices.begin(); iter != vertices.end(); iter++)
        {
            iter->x = iter->x * cos(a) - iter->y * sin(a);
            iter->y = iter->x * sin(a) + iter->y * cos(a);
        }
    }

    void RotateY(float a)
    {
        for(auto iter = vertices.begin(); iter != vertices.end(); iter++)
        {
            iter->x = iter->x * cos(a) + iter->z * sin(a);
            iter->z = iter->x * -sin(a) + iter->z * cos(a);
        }
    }

    void RotateX(float a)
    {
        for(auto iter = vertices.begin(); iter != vertices.end(); iter++)
        {
            iter->y = iter->y * cos(a) - iter->z * sin(a);
            iter->z = iter->y * sin(a) + iter->z * cos(a);
        }
    }
private:
    std::fstream fileReader;
    std::vector<Vec3f> vertices;
    std::vector<std::vector<int>> faces;
protected:
};

#endif