#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#pragma once

#include "common_t1.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/gtx/quaternion.hpp"
//#include "glm/gtx/quaternion.inl"

class Renderable: public AbsObject {
protected:
	// father already defined in 
	vector<Renderable*> sons;

	glm::vec3 pos;
	glm::quat rot;//TODO: try use Pauli matrices(SU(2)) to do the rotation :-)
	glm::vec3 scl{1.0f};
public:
	Renderable() {};
	~Renderable() {};
	virtual void render() = 0;
	void addSon(Renderable* renderable);

	// translate goes in GC units, while rotate and scale applies in local coordinates
	void translate(glm::vec3 v) { this->pos += v; }
	void rotate(glm::quat q) { this->rot = q * this->rot; }
	void rotate(float angle, glm::vec3 axis) { 
		glm::quat q = glm::angleAxis(angle, axis);
		this->rot = q * this->rot;
	}
	// TODO: only uniform scale is supported now, to prevent troubles with normal mappings
	void scale(glm::vec3 v) { this->scl *= v[0]; }
	void scale(float s) { this->scl *= s; }
	glm::mat4 getModelMatrix() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->pos);
		// turn quaternion into rotation matrix
		model = model * glm::toMat4(this->rot);
		
		model = glm::scale(model, this->scl);
		return model;
	}
	// apply model transform to all vertices, and clean the transform
	void applyModelTransform() {
		//TODO: not implemented yet
		throw runtime_error("Not implemented yet!");
	}
};

class Node: public Renderable {
public:
	virtual void render();
};

class ColorfulStone: public Renderable {
private:
	Shader* shader{nullptr};

	Texture* texture{nullptr};

	//vertex data
	float vertices[32] = {
		// positions           // colors           // texture coords
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[6] = {  // note that we start from 0!
		0, 2, 1,
		0, 3, 2
	};
	float texCoords[6] = {
		0.0f, 0.0f,  // lower-left corner  
		1.0f, 0.0f,  // lower-right corner
		0.5f, 1.0f   // top-center corner
	};
	//generate VBO/EBO presets via VAO
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
public:
	ColorfulStone();
	~ColorfulStone();
	void initShader();
	void setShader(Shader* s);
	//Shader* getShader();
	void initTexture();
	virtual void render();
};


class Cube: public Renderable {
private:
	Shader* shader{nullptr};
	Texture* texture{nullptr};
	float vert_data[180] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	unsigned int VAO, VBO;
	
	void initTexture();
	void initShader();
public:
	Cube();
	~Cube();
	virtual void render();
};

class LightCube: public Renderable {
private:
	Shader* shader{nullptr};
	float vertices[8*3]; // generate dynamically
	unsigned int indices[6*2*3]; // render using GL_QUAD

	unsigned int VAO, VBO, EBO;

	glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
	glm::vec3 lightColor{1.0f, 1.0f, 1.0f};
	float viewport_size{0.2f};

	void initShader();
public:
	LightCube();
	~LightCube();
	virtual void render();

	void setLightPos(glm::vec3 pos);
	void setLightColor(glm::vec3 color);
	void setLightUniform(Shader& shader);
	void setViewportSize(float size);
};

class ViewportGrid: public Renderable {
protected:
	unsigned int VAO, VBO, EBO;
	Shader* shader {nullptr};

	int span;

	inline int get_nr_vert() {
		// *2: x and z direction; *2: two vertices per line.
		return 2*2*(span*2+1);
	}
public:
	ViewportGrid(int span=10);

	virtual void render();
};

#endif