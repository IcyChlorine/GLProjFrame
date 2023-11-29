#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#pragma once

#include <iostream>
#include <string>
using namespace std;

#include "assimp/material.h"

#include "common_t1.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

// the most basic bling-phong material
class mesh_loading_err :public runtime_error {
public:
	mesh_loading_err(const char* message="") :
		runtime_error(message) {}
};
inline void ai_chk(aiReturn res) {
    if (res != aiReturn_SUCCESS) {
        error("aiReturn is not aiReturn_SUCCESS");
        throw mesh_loading_err("Error: aiReturn is not aiReturn_SUCCESS");
    }
}

/* 
 * Some subtlty here with circular reference:
 * Suppose Mesh.h include Material.h, and Material.h tries to include Mesh.h
 *   to get definition of class Model. This will FAIL because _MESH_H has been
 *   already been defined in Mesh.h first place, and Material.h will LOSE sight
 *   on definition of class Model. 
 * If we remove the _MESH_H macro, there will be redifinition errors when linking.
 * So define class Model here again.
 */
class Model;

class Material: AbsObject {
private:
    float fac_ambient  { 1.0f };
    float fac_diffuse  { 1.0f };
    float fac_specular { 1.0f };
    float shininess    { 1.0f };
    // not supported yeet
    //float fac_emissive { 1.0f };
    //float opacity      { 1.0f };
    //float refr_index   { 1.0f };

    Texture* tex_ambient  { nullptr };
    Texture* tex_diffuse  { nullptr };
    Texture* tex_specular { nullptr };
    Texture* tex_normal   { nullptr };
    Texture* tex_opacity  { nullptr };

    // for in-class initialization of static members, constexpr is required (const is not enough)
    static constexpr int nr_supported_textypes = 5;
    //TODO: Normal maps are not working properly, to be fixed
	// See: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    static constexpr aiTextureType supported_textypes[nr_supported_textypes] = {
        aiTextureType_AMBIENT, aiTextureType_DIFFUSE, 
        aiTextureType_SPECULAR, aiTextureType_HEIGHT,
        aiTextureType_OPACITY
    };

    static int nr_instances;
    static Shader* shader;
public:
    // all not implemented yet :-)
    //Material() = default;
    Material(Model* father, const aiMaterial* mat);
    //Material(float fac_ambient, float fac_diffuse, float fac_specular, float shininess) {}**
    ~Material();

    void use();
};




#endif // !_MATERIAL_H_