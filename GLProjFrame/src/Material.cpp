#include "Material.h"
#include "Mesh.h"

// why I need to declare them again?
constexpr int Material::nr_supported_textypes;
constexpr aiTextureType Material::supported_textypes[Material::nr_supported_textypes];

#define USE_OPACITY_MAP  (1<<0)

int Material::nr_instances { 0 };
Shader* Material::shader { nullptr };

std::string utf8_to_gbk(const std::string& utf8_str);

Material::Material(Model* env, const aiMaterial* mat): AbsObject(env) {
	// Note: AI_MATKEY_XXX are macros defined like "balabala",0,0
	//       which occupies *3* parameters
	// Note2: You can't pass a aiShadingMode obj to Get,
	//        otherwise you get an AI_FAILURE and wrong results.
	//        you should pass a int obj instead.
	
	aiReturn res;
	int shading_mode;
	res = mat->Get(AI_MATKEY_SHADING_MODEL, shading_mode);
	if(res != aiReturn_SUCCESS) {
		warning("Can't find shading mode\n");
	} else if(shading_mode != aiShadingMode_Phong) {
		warningf("Shading mode is %d, not aiShadingMode_Phong\n", shading_mode);
	}
	
	// load parameters
	aiColor4D tmp;
	ai_chk( mat->Get(AI_MATKEY_COLOR_AMBIENT, tmp) );
	assert(tmp.r == tmp.g && tmp.g == tmp.b); fac_ambient = tmp.r;
	ai_chk( mat->Get(AI_MATKEY_COLOR_DIFFUSE, tmp) );
	assert(tmp.r == tmp.g && tmp.g == tmp.b); fac_diffuse = tmp.r;
	res = ai_chk( mat->Get(AI_MATKEY_COLOR_SPECULAR, tmp) );
	assert(tmp.r == tmp.g && tmp.g == tmp.b); fac_specular = tmp.r;
	ai_chk( mat->Get(AI_MATKEY_SHININESS, shininess) );
	
	
	// avoid NaN when calculating spec_tex^shininess->0^0->NaN.
	shininess = max(shininess, (float)1e-6);

	// load textures
	//TODO: Normal maps are not working properly, to be fixed
	// See: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	const int nr_types = nr_supported_textypes; 
	const aiTextureType* types = supported_textypes;
	const char* type_names[nr_types] = {
		"ambient", "diffuse", "specular", "normal", "opacity"
	};
	Texture* texs[nr_types] = {
		nullptr, nullptr, nullptr, nullptr, nullptr
	};

	for (int t = 0; t < nr_types; t++) {
		int nr_tex = mat->GetTextureCount(types[t]);
		if (nr_tex > 1) {
			errorf("More than 1 texture for type %s, not supported\n", type_names[t]);
			throw mesh_loading_err("Error: nr_tex > 1");
		}
		if (nr_tex == 0) {
			continue;
		}

		aiString path;
		ai_chk( mat->GetTexture(types[t], 0, &path) );
		string path_str = path.C_Str();
		texs[t] = env->loadTexture(utf8_to_gbk(path_str));
	}
	tex_ambient  = texs[0];
	tex_diffuse  = texs[1];
	tex_specular = texs[2];
	tex_normal   = texs[3];
	tex_opacity  = texs[4];

	// heuristic zone - the following treatments are not theoretically justified, 
	// but they enhance shading in most practical cases.
	// 1. prevent model from being too dark
	fac_ambient = 0.0f; //max(fac_ambient,  0.0f);
	// 2. most models have the same color for ambient and diffuse lights. 
	//    so if ambient texture is not specified, use diffuse texture instead.
	//if(!tex_ambient && tex_diffuse)
		tex_ambient = tex_diffuse;


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
		//delete shader;
		//shader = nullptr;
	}
	// the textures are maintained by class Model as shared resources,
	// so we don't need to free them here.
}

void Material::use() {
	shader->use();

	// set real-valued parameters
	shader->setUniform("fac_ambient", fac_ambient);
	shader->setUniform("fac_diffuse", fac_diffuse);
	shader->setUniform("fac_specular", fac_specular);
	shader->setUniform("shininess", shininess);

	int flags = 0;
	if (tex_opacity) flags |= USE_OPACITY_MAP;
	shader->setUniform("flags", flags);

	// set textures
	const int nr_types = nr_supported_textypes;
	Texture* textures[] = 
		{ tex_ambient, tex_diffuse, tex_specular, tex_normal, tex_opacity };
	const char* sampler_names[] = 
		{ "tex_ambient", "tex_diffuse", "tex_specular", "tex_normal", "tex_opacity"};

	// 保护现场
	GLint prev_active_tex;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &prev_active_tex);

	for(int t=0; t<nr_types; t++) {
		glActiveTexture(GL_TEXTURE0 + t);

		if(not textures[t]) {
			glBindTexture(GL_TEXTURE_2D, 0);
			continue;
		}
		else {
			textures[t]->use();
			//printf("Using texture %s\n", sampler_names[t]);
			shader->setUniform(sampler_names[t], t);
		}
	}

	// 恢复现场
	glActiveTexture(prev_active_tex);
}