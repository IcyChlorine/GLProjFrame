#include "Renderable.h"

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

	// clearify format of vertices array
	// [xyz|rgb|uv]
	int format[] = {3,3,2};
	declare_interleaving_vert_data_layout(3, format);	
	
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