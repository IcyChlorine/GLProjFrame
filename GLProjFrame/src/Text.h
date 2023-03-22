#ifndef TEXT_H
#define TEXT_H

#pragma once

#include "Renderable.h"
#include <ft2build.h>
#include FT_FREETYPE_H

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
};

#endif