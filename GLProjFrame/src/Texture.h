#pragma once

#include "common_t1.h"

//一个Texture包装类
class Texture
{
private:
	unsigned int texture{0};
public:
	static constexpr int PIXEL = 0;// GL_NEAREST;
	static constexpr int SMOOTH = 1;// GL_LINEAR;

	static constexpr int REPEAT = 2;// GL_REPEAT;
	static constexpr int MIRRORED_REPEAT = 3;// GL_MIRRORED_REPEAT;
	static constexpr int STRETCH_EDGE = 4;// GL_CLAMP_TO_EDGE;
	static constexpr int BLANK_BORDER = 5;// GL_CLAMP_TO_BORDER;

	Texture(string filename);
	~Texture();

	void use();
	static void setParam(int texParam);
};

