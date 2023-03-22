#include "Renderable.h"

void get_and_bind_vao(unsigned int * pVAO) {
	glGenVertexArrays(1, pVAO);
	//unsigned int gl_errno = glGetError();
	//errorf("GL error: %d\n",gl_errno);
	glBindVertexArray(*pVAO);
}
void get_and_bind_vbo(unsigned int * pVBO) {
	glGenBuffers(1, pVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *pVBO);
}
void get_and_bind_ibo(unsigned int * pIBO) {
	glGenBuffers(1, pIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *pIBO);
}
void claim_vert_data_format_interleaving(){
	// TODO 
}


void Renderable::addSon(Renderable* renderable) {
	sons.push_back(renderable);
}

void Node::render() {
	for(auto son=sons.begin();son!=sons.end();son++){
		(*son)->render();
	}
}

ColorfulStone::ColorfulStone() {
	get_and_bind_vao(&VAO);

	get_and_bind_vbo(&VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// clearify FORMAT of vertices array
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	get_and_bind_ibo(&EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	this->initTexture();
}

void ColorfulStone::setShader(Shader* s) {
	shader = s;
}
void ColorfulStone::initTexture() {
	texture = new Texture("texture\\stone.bmp");
}
void ColorfulStone::render() {
	shader->use();
	texture->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}