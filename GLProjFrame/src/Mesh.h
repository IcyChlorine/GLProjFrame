#pragma once
#ifndef _MESH_H
#define _MESH_H

#include "common_t1.h"
#include "Renderable.h"
#include "Shader.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp/material.h"

extern Shader* mesh_shader;

class Mesh;

/*
 * Known Limitations:
 * 1. In the case of different node referring to one single mesh, the mesh will be constructed
 *    multiple times(not reused)
 * 2. Node transforms are ignored, meaning that all meshes are attatched to global coordinate
 *    directly.
 * 3. Assume that textures are stored in seperate files. 
 *    Some model file format would embed their texture data directly into model file, though.
*/
class Model: public Renderable {
	friend class Mesh;
public:
	Model(const string& filepath); 
	~Model(); 
	void initMesh(const aiScene* scene, aiNode* node, const vector<string>& loaded_tex);
	void initShader();
	void initMaterialTexture(const aiScene* scene, vector<string>& loaded_tex);
	void bindMeshTexture(Mesh* my_mesh, aiMaterial* mat, const vector<string>& loaded_tex);

	virtual void render();
private:
	vector<Texture*> textures;
	string directory;
};

class Mesh: public Renderable {
public:
	Mesh(aiMesh* ai_mesh);
	~Mesh();
	void setTextureIndex(int amb, int diff, int spcl, int norm);
	virtual void render();
private:
	unsigned int VAO{0}, VBO{0}, EBO{0};

	float* vert_data{nullptr};
	
	size_t nr_vert{0};
	unsigned int* indices{nullptr};
	size_t nr_indices{0};

	// 纹理在父对象textures数组中的下标
	// -1表示该类纹理不可用
	int ambient_tex_idx{-1};
	int diffuse_tex_idx{-1};
	int specular_tex_idx{-1};
	int normal_tex_idx{-1};
};

void print_aiScene_basics(ostream& out, const aiScene* scene, unsigned int verbose_level=0);
void print_aiMesh_basics(ostream& out, const aiMesh* mesh, unsigned int verbose_level=0);
void print_aiMaterial_basics(ostream& out, const aiMaterial* mat, unsigned int verbose_level=0);


#endif /* _MESH_H */