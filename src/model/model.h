#ifndef __MODEL_H_
#define __MODEL_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model
{
public: 
    Model(const char * file);
    Model(){};
    ~Model();
    bool OpenFile(const char * file);
    size_t VerticesSize() const;
    size_t FacesSize() const;
    const std::vector<int>& GetFace(size_t i) const;
	const std::vector<int>& GetFaceTexture(size_t i) const;
	const std::vector<int>& GetFaceNormal(size_t i) const;
    Vec3f GetMax() const;
    Vec3f GetMin() const;
    Vec3f GetVertex(size_t i) const;
	Vec2f GetVertexTexture(size_t i) const;
	Vec3f GetVertexNormal(size_t i) const;
	std::string& GetStat();
	bool LoadDiffuse(const char * file);
private:
    std::fstream fileReader;
    std::vector<Vec3f> vertices;
    std::vector<std::vector<int>> faces;
	std::vector<std::vector<int>> faces_texture;
	std::vector<std::vector<int>> faces_normals;
	std::vector<Vec2f> texture_vertices;
	std::vector<Vec3f> normals;
	TGAImage diffuse;
	bool hasDiffuse{false};
protected:
};

#endif