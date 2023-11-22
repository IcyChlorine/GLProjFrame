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

extern Shader* mesh_shader;

class Mesh;

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

#endif /* _MESH_H */