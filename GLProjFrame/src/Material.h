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

// the most basic bling-phong material
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

    Shader* shader { nullptr };
public:
    // all not implemented yet :-)
    //Material() = default;
    Material(aiMaterial* mat) {}
    ~Material() {}

    void apply() {}
};

#endif // !_MATERIAL_H_