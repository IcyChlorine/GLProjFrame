#include "Renderable.h"
#include "Application.h"
#include "Camera.h"

#include <vector>

void Renderable::addSon(Renderable* renderable) {
	sons.push_back(renderable);
}

void Node::render() {
	for(auto son=sons.begin();son!=sons.end();son++){
		(*son)->render();
	}
}

ColorfulStone::ColorfulStone() {
	this->initShader();

	get_and_bind_vao(&VAO);

	get_and_bind_vbo(&VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// clearify format of vertices array
	// [xyz|rgb|uv]
	vector<int> format = {3,3,2};
	declare_interleaving_vert_data_layout(format);
	
	get_and_bind_ibo(&EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	this->initTexture();
}

void ColorfulStone::setShader(Shader* s) {
	shader = s;
}
void ColorfulStone::initShader() {
	shader = new Shader("src/shaders/vertex.glsl", "src/shaders/frag.glsl");
}
void ColorfulStone::initTexture() {
	texture = new Texture("texture/stone.bmp");
}
void ColorfulStone::render() {
	shader->use();

	shader->setUniformMatrix("model", this->getModelMatrix());

	Camera* camera = theApp->getCamera();
	camera->applyCameraTransform(*shader);
	texture->use();

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

ColorfulStone::~ColorfulStone() {
	delete texture;
	delete shader;
}

void Cube::initTexture() {
	texture = new Texture("texture/bedrock.bmp");
}
void Cube::initShader() {
	shader = new Shader("src/shaders/cube.vert.glsl","src/shaders/cube.frag.glsl");
}

Cube::Cube() {
	this->initShader();
	this->initTexture();

	get_and_bind_vao(&VAO);

	get_and_bind_vbo(&VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);

	vector<int> format = {3, 2};
	declare_interleaving_vert_data_layout(format);

	glBindVertexArray(0);

	// Unbind VBO after unbinding VAO, or 
	// VAO will remember the unbinding!
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
Cube::~Cube() {
	delete texture;
	delete shader;
}

void Cube::render() {
	texture->use();
	shader->use();
	// apply model transform
	shader->setUniformMatrix("model", this->getModelMatrix());
	// apply view and proj transform
	Camera* camera = theApp->getCamera();
	camera->applyCameraTransform(*shader);
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vert_data)/sizeof(float)/5);
	glBindVertexArray(0);
}