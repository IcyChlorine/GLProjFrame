#include "Mesh.h"

// for utf-8 to gbk conversion
#include <iconv.h>

#include "Application.h"
#include "Material.h"

#define VERT_PER_TRIG 3
#define STR_EQ 0

Shader* mesh_shader{nullptr};

Model::Model(const string& filepath) {		
	infof("filepath of model is %s\n", filepath.c_str());
	
	auto slash_idx = filepath.find_last_of("\\");//handle both situations
    if(slash_idx == string::npos) slash_idx = filepath.find_last_of("/");

	directory = filepath.substr(0, slash_idx+1);
	printf("%s\n",directory.c_str());

	Assimp::Importer importer;
	const aiScene* ai_scene = importer.ReadFile(filepath,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	
	if(!ai_scene || (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !ai_scene->mRootNode) {
		errorf("Assimp failed at importing `%s`:\n%s\n", filepath.c_str(), importer.GetErrorString());
		throw exception();
	}
	
	// try to inspect the aiScene
	//printHierachy(ai_scene->mRootNode);
	//print_aiScene_basics(cout, ai_scene, 1);
	
	// for the time being, shader needs to be initiated first(before loading materials)
	// currently, all meshes share the same shader, a.k.a. share the same material model
	this->initShader();

	// load materials
	for(int m=0; m<ai_scene->mNumMaterials; m++) {
		printf("\rLoading materials...(%d/%d)", m+1, ai_scene->mNumMaterials);
		
		if(m<1000) print_aiMaterial_basics(cout, ai_scene->mMaterials[m]);

		Material* mat = new Material(this, ai_scene->mMaterials[m]);
		materials.push_back(mat);
	}

	this->initMesh(ai_scene, ai_scene->mRootNode);

	// the ai_scene(and all the attached resources) will be destroyed
	// upon de-construction of Assimp::Importer
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
	InputManager* input = theApp->getInputManager();
	input->addKeyCallback([&]() {
		info("Reloading mesh shader...\n");
		try {
			Shader* new_shader = new Shader("src/shaders/mesh.vert.glsl","src/shaders/mesh.frag.glsl");
			if(mesh_shader) delete mesh_shader;
			mesh_shader = new_shader;
		} catch(exception& e) {
			warning("Shader reloading failed, continue to use old shader.\n");
			return;
		}
	}, GLFW_KEY_R, InputManager::KEY_PRESS);
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
		//mat->Get(AI_MATKEY_SHININESS, )
		
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
void Model::initMesh(const aiScene* scene, aiNode* node) {
	// for the current node
	for(int i=0; i<node->mNumMeshes; i++) {
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh* my_mesh = new Mesh(this, ai_mesh);
		my_mesh->setFather(this);
		this->sons.push_back((Renderable*)my_mesh);
		// bind textures referenced by ai_mesh.material to my_mesh
		//bindMeshTexture(my_mesh, scene->mMaterials[ai_mesh->mMaterialIndex]);
	}
	// for sons
	for(int i=0; i<node->mNumChildren; i++) {
		initMesh(scene, node->mChildren[i]);
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
	mesh_shader->use();
	// apply model transform
	// only one global model transform is supported for a Model
	mesh_shader->setUniformMatrix("model", this->getModelMatrix());

	for(auto mesh : sons){
		mesh->render();
	}
}

Mesh::Mesh(Model* father, aiMesh* ai_mesh) {
	// get the material
	this->setFather(father);
	Model* env = father;
	// so far, the material index are the same between ai and our objects
	mat = env->materials[ai_mesh->mMaterialIndex];

	// load data from ai_mesh, and convert them to fit our format and memory layout
	nr_vert = ai_mesh->mNumVertices;
	//TODO: It's bug-proning to use these hard-coded expressions to calculate 
	// the stride of data of vertices. Replace them when possible.

	vector<int> vert_format = {3, 3, 2, 3, 3};//pos, norm, tex, tan, bitan
	size_t stride = 0;
	for(int i=0; i<vert_format.size(); i++){
		stride += vert_format[i];
	}

	size_t data_len = nr_vert*stride; // total number of float numbers
	vert_data = new float[data_len];
	memset(vert_data, 0, data_len*sizeof(float));

	nr_indices = ai_mesh->mNumFaces * VERT_PER_TRIG;
	indices = new unsigned int[nr_indices];
	memset(indices, 0, nr_indices*sizeof(unsigned int));
	
	// copy vertex data
	for(int i=0; i<nr_vert; i++) {
		vert_data[i*stride+0] = ai_mesh->mVertices[i].x;
		vert_data[i*stride+1] = ai_mesh->mVertices[i].y;
		vert_data[i*stride+2] = ai_mesh->mVertices[i].z;

		if(ai_mesh->HasNormals()) {
			vert_data[i*stride+3] = ai_mesh->mNormals[i].x;
			vert_data[i*stride+4] = ai_mesh->mNormals[i].y;
			vert_data[i*stride+5] = ai_mesh->mNormals[i].z;
		}
		if(ai_mesh->mTextureCoords[0]) {
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vert_data[i*stride+6] = ai_mesh->mTextureCoords[0][i].x;
			vert_data[i*stride+7] = ai_mesh->mTextureCoords[0][i].y;
			
			//vert_data[i*stride+8] = ai_mesh->mTangents[i].x;
			//vert_data[i*stride+9] = ai_mesh->mTangents[i].y;
			//vert_data[i*stride+10]= ai_mesh->mTangents[i].z;

			//vert_data[i*stride+11]= ai_mesh->mBitangents[i].x;
			//vert_data[i*stride+12]= ai_mesh->mBitangents[i].y;
			//vert_data[i*stride+13]= ai_mesh->mBitangents[i].z;
		}
	}

	// copy index data
	for(int i=0; i<ai_mesh->mNumFaces; i++) {
		indices[i*VERT_PER_TRIG    ] = ai_mesh->mFaces[i].mIndices[0]; 
		indices[i*VERT_PER_TRIG + 1] = ai_mesh->mFaces[i].mIndices[1]; 
		indices[i*VERT_PER_TRIG + 2] = ai_mesh->mFaces[i].mIndices[2]; 
	}

	// OK. Now the data have been prepared, make VAO objects.

	get_and_bind_vao(&VAO);

	get_and_bind_vbo(&VBO);
	get_and_bind_ibo(&EBO);	
	glBufferData(GL_ARRAY_BUFFER, nr_vert*stride*sizeof(float), vert_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nr_indices*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	declare_interleaving_vert_data_layout(vert_format);

	unbind_vao();
	unbind_vbo(); unbind_ibo();
}

Mesh::~Mesh() {
	delete[] vert_data;
	delete[] indices;
}

void Mesh::setTextureIndex(int amb, int diff, int spcl, int norm) {

	/*ambient_tex_idx = amb;
	diffuse_tex_idx = diff;
	specular_tex_idx = spcl;
	normal_tex_idx = norm;*/
}

void Mesh::render() {
	
	//Model* model = (Model*)this->father;
	//printf("%d in %d\n",diffuse_tex_idx, model->textures.size());
	mat->use(mesh_shader);

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

inline string mk_idt(unsigned int indent=1) {
    return string(indent*4, ' ');
}

void print_aiMesh_basics(ostream& out, const aiMesh* mesh, unsigned int verbose_level) {
	out << "aiMesh basic info:" << endl;
	out << mk_idt() << "#Vertices: " << mesh->mNumVertices << endl;
	out << mk_idt() << "#Faces: " << mesh->mNumFaces << endl;
	if(!verbose_level--) return;

	out << mk_idt() << "Has normals";
	out << (mesh->HasNormals() ? "true" : "false") << endl;
	out << mk_idt() << "Has tangents and bitangents: ";
	out << (mesh->HasTangentsAndBitangents() ? "true" : "false") << endl;

	out << mk_idt() << "#UVChannels: " << mesh->GetNumUVChannels() << endl;

	if(!verbose_level--) return;

	if(mesh->mNumBones > 0)
		out << mk_idt() << "#Bones: " << mesh->mNumBones << endl;
	if(mesh->mNumAnimMeshes > 0)
		out << mk_idt() << "#AnimMeshes: " << mesh->mNumAnimMeshes << endl;
}


// thanks copilot's favor
std::string utf8_to_gbk(const std::string& utf8_str) {
    std::string gbk_str;
    char gbk_buf[1024] = {0};
    size_t in_len = utf8_str.size();
    size_t out_len = sizeof(gbk_buf);
    char* in_ptr = (char*)utf8_str.c_str();
    char* out_ptr = gbk_buf;

    iconv_t cd = iconv_open("GBK", "UTF-8");
    if (cd != (iconv_t)-1) {
        if (iconv(cd, &in_ptr, &in_len, &out_ptr, &out_len) != (size_t)-1) {
            gbk_str = gbk_buf;
        }
        iconv_close(cd);
    }

    return gbk_str;
}

void print_aiScene_basics(ostream& out, const aiScene* scene, unsigned int verbose_level) {
	out << "aiScene basic info:" << endl;
	out << mk_idt() << "#Meshes: " << scene->mNumMeshes << endl;
	out << mk_idt() << "#Materials: " << scene->mNumMaterials << endl;

	// traverse among all nodes to get statistics about nodes hierarchy
	vector<int> nr_nodes_per_level;
	int nr_nodes {0};
	function<void(aiNode*, int)> dfs_count = [&](aiNode* node, int level) {
		if(level >= nr_nodes_per_level.size()) {
			nr_nodes_per_level.push_back(0);
		}
		nr_nodes_per_level[level]++;
		nr_nodes++;
		for(int i=0; i<node->mNumChildren; i++) {
			dfs_count(node->mChildren[i], level+1);
		}
	};
	dfs_count(scene->mRootNode, 0);

	out << mk_idt() << "#Nodes: " << nr_nodes << " = ";
	for(int lv=0; lv<nr_nodes_per_level.size(); lv++) {
		if(lv) out << " + ";
		out << nr_nodes_per_level[lv];
	}
	out << endl;

	if(!verbose_level--) return;
	out << mk_idt() << "#Animations: " << scene->mNumAnimations << endl;
	out << mk_idt() << "#(embedded)Textures: " << scene->mNumTextures << endl;
	out << mk_idt() << "#Lights: " << scene->mNumLights << endl;
	out << mk_idt() << "#Cameras: " << scene->mNumCameras << endl;

	// Note: some times you get a utf-8 encoding string for node name,
	//       but consoles' default encoding is gbk for Chinese, so you'll
	//       need to convert the encoding. Thanks copilot for the convert
	//       function, which uses libiconv under the hood.
	function<void(aiNode*, int, int)> dfs_print = [&](aiNode* node, int level, int idx) {
		out << mk_idt(level);
		if(level==0) out << "Root: ";
		else         out << "Node " << idx << ": ";
		out << utf8_to_gbk(node->mName.C_Str())
		    << "(" << node->mNumMeshes << " meshes)" << endl;

		for(int i=0; i<node->mNumChildren; i++) {
			dfs_print(node->mChildren[i], level+1, i);
		}
	};
	out << "Node hierarchy:" << endl;
	dfs_print(scene->mRootNode, 0, 0);
}


// Note: size_t extends to *unsigned* long long, so don't use -1 to initialize it.
static size_t max_matkey_len = 0;

void print_aiMaterialProperty(ostream& out, const aiMaterialProperty* prop, unsigned int verbose_level=0) {
	const unsigned int MAX_TEXTYPE_ENUM = (unsigned int)aiTextureType_UNKNOWN;
	const char* textype_names[MAX_TEXTYPE_ENUM+1];
	for(int i=0; i<=MAX_TEXTYPE_ENUM; i++) 
		textype_names[i] = "Unknown aiTextureType!";
	textype_names[aiTextureType_NONE]       = "aiTextureType_NONE";
	textype_names[aiTextureType_DIFFUSE]    = "aiTextureType_DIFFUSE";
	textype_names[aiTextureType_SPECULAR]   = "aiTextureType_SPECULAR";
	textype_names[aiTextureType_AMBIENT]    = "aiTextureType_AMBIENT";
	textype_names[aiTextureType_EMISSIVE]   = "aiTextureType_EMISSIVE";
	textype_names[aiTextureType_HEIGHT]     = "aiTextureType_HEIGHT";
	textype_names[aiTextureType_NORMALS]    = "aiTextureType_NORMALS";
	textype_names[aiTextureType_SHININESS]  = "aiTextureType_SHININESS";
	textype_names[aiTextureType_OPACITY]    = "aiTextureType_OPACITY";
	textype_names[aiTextureType_DISPLACEMENT]="aiTextureType_DISPLACEMENT";
	textype_names[aiTextureType_LIGHTMAP]   = "aiTextureType_LIGHTMAP";
	textype_names[aiTextureType_REFLECTION] = "aiTextureType_REFLECTION";
	textype_names[aiTextureType_UNKNOWN]    = "aiTextureType_UNKNOWN";
	
	const unsigned int MAX_DATATYPE_ENUM = (unsigned int)aiPTI_Buffer;
	const char* datatype_names[MAX_DATATYPE_ENUM+1];
	for(int i=0; i<=MAX_DATATYPE_ENUM; i++)
		datatype_names[i] = "Unknown dtype!";
	datatype_names[aiPTI_Float]   = "aiPTI_Float";
	//datatype_names[aiPTI_Double]  = "aiPTI_Double";
	datatype_names[aiPTI_String]  = "aiPTI_String";
	datatype_names[aiPTI_Integer] = "aiPTI_Integer";
	datatype_names[aiPTI_Buffer]  = "aiPTI_Buffer";

	const unsigned int MAX_SHADINGMODE_ENUM = (unsigned int)aiShadingMode_Fresnel;
	const char* shadingmode_names[MAX_SHADINGMODE_ENUM+1];
	for(int i=0; i<=MAX_SHADINGMODE_ENUM; i++)
		shadingmode_names[i] = "Unknown shading mode!";
	shadingmode_names[aiShadingMode_Flat]       = "aiShadingMode_Flat";
	shadingmode_names[aiShadingMode_Gouraud]    = "aiShadingMode_Gouraud";
	shadingmode_names[aiShadingMode_Phong]      = "aiShadingMode_Phong";
	shadingmode_names[aiShadingMode_Blinn]      = "aiShadingMode_Blinn";
	shadingmode_names[aiShadingMode_Toon]       = "aiShadingMode_Toon";
	shadingmode_names[aiShadingMode_OrenNayar]  = "aiShadingMode_OrenNayar";
	shadingmode_names[aiShadingMode_Minnaert]   = "aiShadingMode_Minnaert";
	shadingmode_names[aiShadingMode_CookTorrance]="aiShadingMode_CookTorrance";
	shadingmode_names[aiShadingMode_NoShading]  = "aiShadingMode_NoShading";
	shadingmode_names[aiShadingMode_Fresnel]    = "aiShadingMode_Fresnel";
	
	out << "key=" << prop->mKey.C_Str() << "," << string(max_matkey_len-strlen(prop->mKey.C_Str())+1, ' ');
	switch(prop->mType) {
		case aiPTI_Float:
			out << "value = ";
			for(int i=0; i<prop->mDataLength/sizeof(float); i++) {
				if(i) out << ",";
				out << ((float*)prop->mData)[i];
			}
			break;
		case aiPTI_Double:
			out << "value = ";
			for(int i=0; i<prop->mDataLength/sizeof(double); i++) {
				if(i) out << ",";
				out << ((double*)prop->mData)[i];
			}
			out << "(double)";
			break;
		case aiPTI_Integer:
		{
			out << "value = ";
			if(!strcmp(prop->mKey.C_Str(), "$mat.shadingm")) {
				assert(prop->mDataLength == sizeof(int));
				out << shadingmode_names[*(int*)prop->mData];
				out << "(" << *(int*)prop->mData << ")";
				break;
			}
			// else
			for(int i=0; i<prop->mDataLength/sizeof(int); i++) {
				if(i) out << ",";
				out << ((int*)prop->mData)[i];
			}
			out << "(int)";
			break;
		}
		case aiPTI_String:
		{
			/* There's a bug in old version of assimp:
			 * On one hand, aiString.data is 8 bytes after aiString.length when compiled on machine
			 * where size_t is 8 bytes.
			 * On the other hand, most of the time, assimp function takes it for granted that aiString.data
			 * is 4 bytes after aiString.length, accessing it using prop->mData+4, as they assume size_t
			 * must be 4 bytes long.
			 * => So to print the aiString in prop->mData correctly, just print prop->mData+4.
			*/

			string str{ ((aiString*)prop->mData)->data };
			//string str{             prop->mData+4 };

			// if key starts with _AI_MATKEY_TEXTURE_BASE
			if(strncmp(prop->mKey.C_Str(), _AI_MATKEY_TEXTURE_BASE, 
			                       sizeof(_AI_MATKEY_TEXTURE_BASE)-1) == STR_EQ) {
				// identify a texture in file
				out << "texture [type = " << textype_names[prop->mSemantic] << ","
				                          << prop->mIndex << ", ";
				out << "path = \"" << utf8_to_gbk(str) << "\"]";
			} else {
				// otherwise, the string is a value
				out << "value = \"" << utf8_to_gbk(str) << "\"";
			}
			
			break;
			
		}
		case aiPTI_Buffer:
			out << "value = " << "(binary data)";
			break;
		default:
			out << "value = " << "(Unknown type: " << prop->mType << ")";
			break;
		
	}
	//out << "semantic=" << textype_names[prop->mSemantic] << ", ";
	//out << "type=" << datatype_names[prop->mType] << ", ";
	//out << "texture index=" << prop->mIndex << ", ";
	out << endl;
}

void print_aiMaterial_basics(ostream& out, const aiMaterial* mat, unsigned int verbose_level) {
	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	out << "basic info of aiMaterial \"" << utf8_to_gbk(name.C_Str()) << "\":" << endl;
	out << mk_idt() << "#Properties = " << mat->mNumProperties << endl;

	// pre-compute the max length of aiMaterialProperty::mKey, for better printing
	// Note: the dependency on max_matkey_len makes this function not thread-safe.
	max_matkey_len = 0;
	for(int i=0; i<mat->mNumProperties; i++) {
		const aiMaterialProperty* prop = mat->mProperties[i];
		if(strlen(prop->mKey.C_Str()) > max_matkey_len) {
			max_matkey_len = strlen(prop->mKey.C_Str());
		}
	}

	for(int i=0; i<mat->mNumProperties; i++) {
		const aiMaterialProperty* prop = mat->mProperties[i];
		out << mk_idt() << "Property " << i << ": ";
		print_aiMaterialProperty(out, prop);
		//out << endl;
	}

	max_matkey_len = 0;
}