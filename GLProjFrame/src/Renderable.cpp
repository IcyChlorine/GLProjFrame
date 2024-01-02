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
	input->addKeyCallback([&]() {
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
	//glLineWidth(4.0f);
	glDrawArrays(GL_LINES, 0, get_nr_vert());
	glBindVertexArray(0);
}

// insert bit at position p into val
// high bits of val will be moved to the left by 1 bit
// while low bits of val remain unchanged
int insbit(int val, int p, int bit) {
	
	int lm = (1<<p)-1;
	int hm = (~0)- lm;

	int l = val & lm;
	int h = val & hm;
	
	return (h<<1) | (bit<<p) | l;
}

int gray(int i) {
	return i ^ (i>>1);
}

LightCube::LightCube() {
	// generate vertex and index data dynamically
	for(int i=0;i<8;i++) {
		for(int x=0;x<3;x++) {
			vertices[i*3+x] = (i&(1<<x))?0.5f:-0.5f;
			printf("%f ", vertices[i*3+x]);
		}
		cout << endl;
	}

	// a bit fancy trick here to generate indices
	constexpr int nr_quad = 6;
	for(int x=0;x<3;x++) {
		for(int i=0;i<4;i++) {
			indices[x*8  +i] = insbit(gray(i), x, 0);
			indices[x*8+4+i] = insbit(gray(i), x, 1);
		}
	}
	// OpenGL has removed GL_QUADS since 3.1, so we have to convert to triangles
	unsigned indices_quad[6*4];
	memcpy(indices_quad, indices, sizeof(indices_quad));
	memset(indices, 0, sizeof(indices));
	for(int face=0;face<6;face++) {
		int tri = face*2;
		indices[tri*3+0] = indices_quad[face*4+0];
		indices[tri*3+1] = indices_quad[face*4+1];
		indices[tri*3+2] = indices_quad[face*4+2];
		tri = face*2+1;
		indices[tri*3+0] = indices_quad[face*4+0];
		indices[tri*3+1] = indices_quad[face*4+2];
		indices[tri*3+2] = indices_quad[face*4+3];
	}
	

	this->initShader();

	init_buffers(&VAO, &VBO, &EBO);

	vector<int> format = {3};
	declare_interleaving_vert_data_layout(format);

	set_vbo_data(vertices, sizeof(vertices));
	set_ibo_data(indices, sizeof(indices));

	unbind_buffers();

	this->pos = this->lightPos;
	this->scl = glm::vec3(viewport_size);
}

LightCube::~LightCube() {
	delete shader;
}

void LightCube::initShader() {
	shader = new Shader("src/shaders/light_cube.vert.glsl", "src/shaders/light_cube.frag.glsl");
}

void LightCube::render() {
	shader->use();

	shader->setUniformMatrix("model", this->getModelMatrix());
	Camera* camera = theApp->getCamera();
	camera->applyCameraTransform(*shader);

	shader->setUniform("light_color", this->lightColor);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void LightCube::setLightPos(glm::vec3 pos) {
	this->lightPos = pos;
	this->pos = pos;
}

void LightCube::setLightColor(glm::vec3 color) {
	this->lightColor = color;
}

void LightCube::setViewportSize(float size) {
	this->viewport_size = size;
	this->scl = glm::vec3(size);
}

void LightCube::setLightUniform(Shader& shader) {
	shader.setUniform("light_pos", this->lightPos);
	shader.setUniform("light_color", this->lightColor);
}