#pragma once
#ifndef _MESH_H
#define _MESH_H

#include <vector>
using std::vector;

#include "glpp/util.h"
#include "glpp/Texture.h"

#include "assimp/Importer.hpp"     // C++ importer interface
#include "assimp/scene.h"       // Output data structure
#include "assimp/postprocess.h" // Post processing flags
#pragma comment(lib, "lib/assimp-vc140-mt.lib")


class Mesh:public AbsObject{//internal class
friend class Scene;
public:
    Mesh(){}
    Mesh(const aiMesh* pAiMesh){init(pAiMesh);}
    ~Mesh();

    void init(const aiMesh* pAiMesh);
    void render();
private:
    int numVertices{0},numIndices{0};
    int materialIndex{-1};

    GLuint VAO;
    //顶点数据格式：(x,y,z),(nx,ny,nz),(s,t)=x,y,z,nx,ny,nz,s,t
    //顶点索引为vi的顶点，数据下标范围为[8*vi, 8*vi+7]=[8*vi,8*vi+8)
    float* vertices{nullptr};
    unsigned* indices{nullptr};
};

class Scene:public AbsObject
{
public:
    Scene(){}
    Scene(const string& filename){init(filename);}
    Scene(const aiScene* pAiScene, const string& filename=""){init(pAiScene, filename);}
    ~Scene();

    void init(const string& filename);
    void init(const aiScene* pAiScene, const string& filename="");

    void render();
private:
    bool loaded{false};
    void clear(){}//TODO: not implemented

    vector<Mesh> meshes;
    vector<Texture*> materials;
    
};


#endif