#include "Mesh.h"
#include "Application.h"
#define VERT_PER_TRIG 3

Shader* mesh_shader{nullptr};

Model::Model(const string& filepath) {		
	// TODO: always '/' and no '\\'?
	infof("filepath of model is %s\n", filepath.c_str());
	
	directory = filepath.substr(0, filepath.find_last_of('/')+1);
	printf("%s\n",directory.c_str());

	Assimp::Importer importer;
	const aiScene* ai_scene = importer.ReadFile(filepath,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	
	if(!ai_scene || (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !ai_scene->mRootNode) {
		errorf("Assimp failed at importing `%s`:\n%s\n", filepath.c_str(), importer.GetErrorString());
		throw exception();
	}
	//printHierachy(ai_scene->mRootNode);
	//system("pause");

	vector<string> loaded_tex;
	this->initMaterialTexture(ai_scene, loaded_tex);
	this->initMesh(ai_scene, ai_scene->mRootNode, loaded_tex);
	// currently, all meshes share the same shader, a.k.a. share the same material model
	this->initShader();
}

Model::~Model() {
	for(auto it=textures.begin(); it!=textures.end(); it++)
		delete *it;
	for(auto it=sons.begin(); it!=sons.end(); it++)
		delete *it;
	if(mesh_shader) delete mesh_shader;
}
void Model::initShader() {
	if(!mesh_shader) {
		mesh_shader = new Shader("src/shaders/mesh.vert.glsl","src/shaders/mesh.frag.glsl");
	}
}

// load all textures referenced by scene->mMaterials into this->textures
void Model::initMaterialTexture(const aiScene* scene, vector<string>& loaded_tex) {
	// supported texture types
	// at most 1 texture per type
	aiTextureType types[]={
		aiTextureType_DIFFUSE, aiTextureType_SPECULAR, 
		aiTextureType_HEIGHT, aiTextureType_AMBIENT
	};

	// for all materials
	for(int idx=0; idx<scene->mNumMaterials; idx++) {
		aiMaterial* mat = scene->mMaterials[idx];
		// for all supported texture types	
		for (int t = 0; t < 4; t++) {
			aiTextureType tex_type = types[t];
			if (mat->GetTextureCount(tex_type) == 0) {
				continue;
			}
			if (mat->GetTextureCount(tex_type) > 1) {
				error("Mesh with more than 1 texture for a given texture type! We can't handle this.\n");
				throw exception();
			}
			// exactly one texture for tex_type

			// get the filename of texture image file, like "stone.bmp"
			aiString filename;
			mat->GetTexture(tex_type, 0, &filename);

			// check if texture has been loaded before
			// if so, reuse the loaded texture, avoid re-loading
			auto it = loaded_tex.begin();
			if ((it=find(loaded_tex.begin(), loaded_tex.end(), filename.C_Str())) != loaded_tex.end()) {
				infof("Find duplicate texture `%s` when loading materials.\n", filename.C_Str());
				continue;
			} 

			/* * finally load the texture! */	
			Texture *texture = new Texture(directory + filename.C_Str());
			// default texture parameters
			texture->setParam(Texture::SMOOTH);
			texture->setParam(Texture::REPEAT);
			textures.push_back(texture);
			
			loaded_tex.push_back(filename.C_Str());
		}
	}
}

// init meshes referenced by `node` and its children
void Model::initMesh(const aiScene* scene, aiNode* node, const vector<string>& loaded_tex) {

	// 
	for(int i=0; i<node->mNumMeshes; i++) {
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh* my_mesh = new Mesh(ai_mesh);
		my_mesh->setFather((AbsObject*)this);
		sons.push_back((Renderable*)my_mesh);
		// bind textures referenced by ai_mesh.material to my_mesh
		bindMeshTexture(my_mesh, scene->mMaterials[ai_mesh->mMaterialIndex], loaded_tex);
	}
	for(int i=0; i<node->mNumChildren; i++) {
		initMesh(scene, node->mChildren[i], loaded_tex);
	}
}

// bind textures referenced by `mat` to `my_mesh`.
void Model::bindMeshTexture(Mesh* my_mesh, aiMaterial* mat, const vector<string>& loaded_tex) {
	
	// The following texture types are supported/considered,
	aiTextureType types[]={
		aiTextureType_DIFFUSE, aiTextureType_SPECULAR, 
		aiTextureType_HEIGHT, aiTextureType_AMBIENT
	};

	// the index of texture in Model.textures, to be found by filename
	int tex_idx[] = {-1, -1, -1, -1};

	for (int t = 0; t < 4; t++) {
		aiTextureType tex_type = types[t];
		if (mat->GetTextureCount(tex_type) == 0) {
			continue;
		}
		if (mat->GetTextureCount(tex_type) > 1) {
			error("Mesh with more than 1 texture for a given texture type! We can't handle this.\n");
			throw exception();
		}
		// exactly one texture for tex_type

		// get the filename of texture image file, like "stone.bmp"
		aiString filename;
		mat->GetTexture(tex_type, 0, &filename);

		auto it = loaded_tex.begin();
		if ((it=find(loaded_tex.begin(), loaded_tex.end(), filename.C_Str())) != loaded_tex.end()) {
			infof("Find duplicate texture `%s`.\n", filename.C_Str());
			tex_idx[t] = it-loaded_tex.begin();
		} else {
			errorf("Texture `%s` not found in loaded textures.\n", filename.C_Str());
			throw exception();
		}

		for(auto s : loaded_tex){
			cout << s << ", ";
		}
		cout << endl;
	}

	// bind to my_mesh
	my_mesh->setTextureIndex(tex_idx[0], tex_idx[1], tex_idx[2], tex_idx[3]);
}

void Model::render() {
	for(auto mesh : sons){
		mesh->render();
	}
}

Mesh::Mesh(aiMesh* ai_mesh) {
	// load data from ai_mesh, and convert them to fit our format and memory layout

	nr_vert = ai_mesh->mNumVertices;
	//TODO: It's bug-proning to use these hard-coded expressions to calculate 
	// the stride of data of verteces. Replace them when possible.
	size_t varr_slots = nr_vert*(3+3+2+3+3);
	vert_data = new float[varr_slots];
	memset(vert_data, 0, varr_slots*sizeof(float));

	nr_indices = ai_mesh->mNumFaces * VERT_PER_TRIG;
	indices = new unsigned int[nr_indices];
	memset(indices, 0, nr_indices*sizeof(unsigned int));

	
	for(int i=0; i<nr_vert; i++) {
		vert_data[i*14+0] = ai_mesh->mVertices[i].x;
		vert_data[i*14+1] = ai_mesh->mVertices[i].y;
		vert_data[i*14+2] = ai_mesh->mVertices[i].z;

		if(ai_mesh->HasNormals()) {
			vert_data[i*14+3] = ai_mesh->mNormals[i].x;
			vert_data[i*14+4] = ai_mesh->mNormals[i].y;
			vert_data[i*14+5] = ai_mesh->mNormals[i].z;
		}
		if(ai_mesh->mTextureCoords[0]) {
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vert_data[i*14+6] = ai_mesh->mTextureCoords[0][i].x;
			vert_data[i*14+7] = ai_mesh->mTextureCoords[0][i].y;
			
			//vert_data[i*14+8] = ai_mesh->mTangents[i].x;
			//vert_data[i*14+9] = ai_mesh->mTangents[i].y;
			//vert_data[i*14+10]= ai_mesh->mTangents[i].z;

			//vert_data[i*14+11]= ai_mesh->mBitangents[i].x;
			//vert_data[i*14+12]= ai_mesh->mBitangents[i].y;
			//vert_data[i*14+13]= ai_mesh->mBitangents[i].z;
		}
	}

	for(int i=0; i<ai_mesh->mNumFaces; i++) {
		indices[i*VERT_PER_TRIG    ] = ai_mesh->mFaces[i].mIndices[0]; 
		indices[i*VERT_PER_TRIG + 1] = ai_mesh->mFaces[i].mIndices[1]; 
		indices[i*VERT_PER_TRIG + 2] = ai_mesh->mFaces[i].mIndices[2]; 
	}

	// OK. Now the data have been prepared, make VAO objects.

	get_and_bind_vao(&VAO);

	get_and_bind_vbo(&VBO);
	get_and_bind_ibo(&EBO);	
	glBufferData(GL_ARRAY_BUFFER, nr_vert*(3+3+2+3+3)*sizeof(float), vert_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nr_indices*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	int vert_format[] = {3, 3, 2, 3, 3};//pos, norm, tex, tan, bitan
	declare_interleaving_vert_data_layout(5, vert_format);

	unbind_vao();
	unbind_vbo(); unbind_ibo();
}

Mesh::~Mesh() {
	delete[] vert_data;
	delete[] indices;
}

void Mesh::setTextureIndex(int amb, int diff, int spcl, int norm) {
	ambient_tex_idx = amb;
	diffuse_tex_idx = diff;
	specular_tex_idx = spcl;
	normal_tex_idx = norm;
}

void Mesh::render() {
	mesh_shader->use();
	Model* model = (Model*)this->father;
	//printf("%d in %d\n",diffuse_tex_idx, model->textures.size());
	glActiveTexture(GL_TEXTURE0);
	mesh_shader->setUniform("ambient",0);
	if(ambient_tex_idx!=-1) {
		model->textures[ambient_tex_idx]->use();
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE1);
	mesh_shader->setUniform("diffuse",1);
	if(diffuse_tex_idx!=-1) {
		model->textures[diffuse_tex_idx]->use();
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE2);
	mesh_shader->setUniform("specular",2);
	if(specular_tex_idx!=-1) {
		model->textures[specular_tex_idx]->use();
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE3);
	mesh_shader->setUniform("normal",3);
	if(normal_tex_idx!=-1) {
		model->textures[normal_tex_idx]->use();
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	Camera* camera = theApp->getCamera();
	camera->applyCameraTransform(*mesh_shader);
	glUniform3fv(
		glGetUniformLocation(mesh_shader->m_shaderID, "f_camera_pos"), 1, 
		glm::value_ptr(((GameCamera*)camera)->getPosition())
	);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nr_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}
