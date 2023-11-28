#include "Material.h"
#include "Mesh.h"

int Material::nr_instances { 0 };
Shader* Material::shader { nullptr };

Material::Material(Model* env, const aiMaterial* mat): AbsObject(env) {
	// Note: AI_MATKEY_XXX are macros defined like "balabala",0,0
	//       which occupies *3* parameters
	// Note2: You can't pass a aiShadingMode obj to Get,
	//        otherwise you get an AI_FAILURE and wrong results.
	//        you should pass a int obj instead.
	
	int shading_mode;
	ai_chk( mat->Get(AI_MATKEY_SHADING_MODEL, shading_mode) );
	if(shading_mode != aiShadingMode_Phong) {
		warningf("Shading mode is %d, not aiShadingMode_Phong", shading_mode);
	}
	
	//assert(shading_mode == aiShadingMode_Phong);
	// load parameters
	ai_chk( mat->Get(AI_MATKEY_COLOR_AMBIENT, fac_ambient) );
	ai_chk( mat->Get(AI_MATKEY_COLOR_DIFFUSE, fac_diffuse) );
	ai_chk( mat->Get(AI_MATKEY_COLOR_SPECULAR, fac_specular) );
	ai_chk( mat->Get(AI_MATKEY_SHININESS, shininess) );

	const int nr_types = 4;

	// load textures
	aiTextureType types[nr_types]={
		aiTextureType_AMBIENT, aiTextureType_DIFFUSE, 
		aiTextureType_SPECULAR, aiTextureType_NORMALS
	};
	const char* type_names[nr_types] = {
		"ambient", "diffuse", "specular", "normal"
	};
	Texture* texs[nr_types] = {
		nullptr, nullptr, nullptr, nullptr
	};

	for (int t = 0; t < nr_types; t++) {
		int nr_tex = mat->GetTextureCount(types[t]);
		if (nr_tex > 1) {
			errorf("More than 1 texture for type %s, not supported", type_names[t]);
			throw mesh_loading_err("Error: nr_tex > 1");
		}
		if (nr_tex == 0) {
			continue;
		}

		aiString path;
		ai_chk( mat->GetTexture(types[t], 0, &path) );
		string path_str = path.C_Str();
		texs[t] = env->loadTexture(path_str);
	}
	tex_ambient  = texs[0];
	tex_diffuse  = texs[1];
	tex_normal   = texs[2];
	tex_specular = texs[3];

	// load shader
	if(!shader) {
		//shader = new Shader("shader/Phong.vert", "shader/Phong.frag");
		// use the same shader as mesh for the time being
		shader = mesh_shader;
	}
	nr_instances++;
}

Material::~Material() {
	nr_instances--;
	if(nr_instances == 0) {
		delete shader;
		shader = nullptr;
	}
}

void Material::use() {
	shader->use();

	// set real-valued parameters
	shader->setUniform("fac_ambient", fac_ambient);
	shader->setUniform("fac_diffuse", fac_diffuse);
	shader->setUniform("fac_specular", fac_specular);
	shader->setUniform("shininess", shininess);

	// set textures
	Texture* textures[] = 
		{ tex_ambient, tex_diffuse, tex_specular, tex_normal };
	const char* sampler_names[] = 
		{ "tex_ambient", "tex_diffuse", "tex_specular", "tex_normal" };

	for(int t=0; t<4; t++) {
		if(not textures[t])
			continue;
		glActiveTexture(GL_TEXTURE0 + t);
		textures[t]->use();
		shader->setUniform(sampler_names[t], t);
	}
}