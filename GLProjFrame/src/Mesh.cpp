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
	this->initMesh(ai_scene, ai_scene->mRootNode);

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

void Model::initMesh(const aiScene* scene, aiNode* node) {
	vector<string> loaded_tex;

	for(int i=0; i<node->mNumMeshes; i++) {
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh* my_mesh = new Mesh(ai_mesh);
		my_mesh->setFather((AbsObject*)this);
		sons.push_back((Renderable*)my_mesh);
		//load material textures related to my_mesh and specify texture index for if
		//repeated textures are loaded only once by checkint `loaded_tex`.
		initMaterial(my_mesh, scene->mMaterials[ai_mesh->mMaterialIndex], loaded_tex);
	}
	for(int i=0; i<node->mNumChildren; i++) {
		initMesh(scene, node->mChildren[i]);
	}
}

void Model::initMaterial(Mesh* my_mesh, aiMaterial* mat, vector<string>& loaded_tex) {

	aiTextureType types[]={
		aiTextureType_DIFFUSE, aiTextureType_SPECULAR, 
		aiTextureType_HEIGHT, aiTextureType_AMBIENT
	};

	int tex_idx[] = {-1, -1, -1, -1};

	for (int j = 0; j < 4; j++) {
		auto type = types[j];
		if (mat->GetTextureCount(type) == 0) {
			continue;
		}
		if (mat->GetTextureCount(type) > 1) {
			error("Mesh with more than 1 texture for a given texture type! We can't handle this.\n");
			throw exception();
		}

		aiString filename;
		mat->GetTexture(type, 0, &filename);

		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		
		auto it = loaded_tex.begin();
		if ((it=find(loaded_tex.begin(), loaded_tex.end(), filename.C_Str())) != loaded_tex.end()) {
			tex_idx[j] = it-loaded_tex.begin();
		} else {
			Texture *texture = new Texture(directory + filename.C_Str());
			texture->use();
			texture->setParam(Texture::SMOOTH);
			texture->setParam(Texture::REPEAT);
			textures.push_back(texture);
			tex_idx[j] = textures.size()-1;
		}

		loaded_tex.push_back(filename.C_Str());
	}
	my_mesh->setTextureIndex(tex_idx[0],tex_idx[1],tex_idx[2],tex_idx[3]);
}

void Model::render() {
	for(auto it=sons.begin(); it!=sons.end(); it++){
		(*it)->render();
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
