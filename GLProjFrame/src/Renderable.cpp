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


ViewportGrid::ViewportGrid(int span) {
	this->span = span;
	float* grid_vert_data = new float[get_nr_vert()*3];
	float* p = grid_vert_data;

	// construct the lines manually
	for(int x=-span; x<=span; x++) {
		p[0] = x;
		p[1] = 0;
		p[2] = -span;
		p += 3;
		p[0] = x;
		p[1] = 0;
		p[2] = span;
		p += 3;
	}
	for(int z=-span; z<=span; z++) {
		p[0] = -span;
		p[1] = 0;
		p[2] = z;
		p += 3;
		p[0] = span;
		p[1] = 0;
		p[2] = z;
		p += 3;
	}

	get_and_bind_vao(&VAO);
	get_and_bind_vbo(&VBO);

	vector<int> format = {3};
	declare_interleaving_vert_data_layout(format);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*get_nr_vert()*3, grid_vert_data, GL_STATIC_DRAW);
	delete grid_vert_data;

	unbind_vao();
	unbind_vbo();

	shader = new Shader("src/shaders/viewport_grid.vert.glsl", "src/shaders/viewport_grid.frag.glsl");
	InputManager* input = theApp->getInputManager();
	input->setKeyCallback([&]() {
		info("Reloading grid shader...\n");
		try {
			Shader* new_shader = new Shader("src/shaders/viewport_grid.vert.glsl","src/shaders/viewport_grid.frag.glsl");
			if(shader) delete shader;
			shader = new_shader;      
		} catch(exception& e) {
			warning("Shader reloading failed, continue to use old shader.\n");
			return;
		}
	}, GLFW_KEY_R, InputManager::KEY_PRESS);
}

void ViewportGrid::render() {
	shader->use();

	shader->setUniformMatrix("model", this->getModelMatrix());
	Camera* camera = theApp->getCamera();
	camera->applyCameraTransform(*shader);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, get_nr_vert());
	glBindVertexArray(0);
}