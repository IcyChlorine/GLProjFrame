#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include "common_t1.h"
#include "Shader.h"
#include "Texture.h"

extern vector<bool> depth_stack;

class Renderable: public AbsObject {
protected:
	// father already defined in 
	vector<Renderable*> sons;
public:
	Renderable() {};
	~Renderable() {};
	virtual void render() = 0;
	void addSon(Renderable* renderable);
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
	void setShader(Shader* s);
	void initTexture();
	virtual void render();
};

#endif