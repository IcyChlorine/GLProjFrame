#pragma once
#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <map>
using namespace std;

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"

#include "common_t1.h"
#include "Renderable.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

extern Shader* mesh_shader;

// extra declaration(other than including Material.h) here
// for circular reference. See Material.h for more explanation.
class Material;
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
	void initMesh(const aiScene* scene, aiNode* node);
	void initShader();
	void initMaterialTexture(const aiScene* scene, vector<string>& loaded_tex);
	void bindMeshTexture(Mesh* my_mesh, aiMaterial* mat, const vector<string>& loaded_tex);

	const Texture* getTexture(const string& name) const {
		return textures[tex_name_to_idx.at(name)];
	}
	const Texture* getTexture(int idx) const {
		return textures[idx];
		//TODO: add check
	}
	Texture* loadTexture(const string& name) {
		// if texture `name` already loaded
		if (tex_name_to_idx.find(name) != tex_name_to_idx.end()) {
			return textures[tex_name_to_idx.at(name)];
		}
		else { // new texture
			Texture* tex = new Texture(directory+name);
			tex->setParam(Texture::SMOOTH);
			tex->setParam(Texture::REPEAT);
			textures.push_back(tex);
			tex_name_to_idx[name] = textures.size() - 1;
			return tex;
		}
	}

	virtual void render();
private:
	map<string, int> tex_name_to_idx;
	vector<Texture*> textures;
	vector<Material*> materials;
	string directory;
};

class Mesh: public Renderable {
public:
	Mesh(Model* father, aiMesh* ai_mesh);
	~Mesh();
	void setTextureIndex(int amb, int diff, int spcl, int norm);
	virtual void render();
private:
	unsigned int VAO{0}, VBO{0}, EBO{0};

	float* vert_data{nullptr};
	
	size_t nr_vert{0};
	unsigned int* indices{nullptr};
	size_t nr_indices{0};

	Material* mat {nullptr};
};

// utils for print assimp stuff and debug
void print_aiScene_basics(ostream& out, const aiScene* scene, unsigned int verbose_level=0);
void print_aiMesh_basics(ostream& out, const aiMesh* mesh, unsigned int verbose_level=0);
void print_aiMaterial_basics(ostream& out, const aiMaterial* mat, unsigned int verbose_level=0);

string utf8_to_gbk(const std::string& utf8_str);

#endif /* _MESH_H */