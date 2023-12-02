#ifndef TEXT_H
#define TEXT_H

#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H

#include "Renderable.h"

class Text: public Renderable
{
public:
    Text(string text="", glm::ivec2 pos=glm::ivec2(0,0), float scale=1.0f);
    ~Text();
	virtual void render();
	
	void setText(string text);
	void setPosition(glm::ivec2 pos);
	void setPosition(unsigned int x, unsigned int y);
private:
	string text;
	glm::ivec2 pos;
	float scale;

	//generate VBO/EBO presets via VAO
	unsigned int VAO;
	unsigned int VBO;
	float vert_data[16] = {
		-1.0f, -1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  0.0f,  1.0f,
	};

	/* Note: Construct a global variable like "Shader text_shader("...","...");"
	 *       is NOT reliable, as global variables are constructed when the program starts,
	 *       by which time OpenGL may not be initialized yet.
	 */
	static int nr_instances;
	static Shader* text_shader;
};

#endif