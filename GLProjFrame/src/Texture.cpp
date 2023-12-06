#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(string filename)
{
	//load texture
	gen_and_bind_texture(&texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	set_texture_param(GL_NEAREST);

	// load and generate the texture
	int width{0}, height{0}, nr_channels{0};
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nr_channels, 0);
	if (data == nullptr)
	{
		errorf("Failed to load texture from file `%s`.\n", filename.c_str());
		throw runtime_error("Failed to load texture from file.");
	}
	verbosef("#channel of `%s` is %d.\n",filename.c_str(),nr_channels);

	GLenum format;
	if     (nr_channels==1) format=GL_RED;
	else if(nr_channels==3) format=GL_RGB;
	else if(nr_channels==4) format=GL_RGBA;
	else { error("Unsupported #channel!\n"); throw exception(); }

	simplifid_tex_image_func(data, format, width, height);
	
	stbi_image_free(data); // data have been copied to OpenGL, so we can free `data` now
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::~Texture()
{
}


void Texture::use()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}


void Texture::setParam(int texParam)
{
	GLint prev_tex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_tex);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (texParam == PIXEL) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}else if(texParam == SMOOTH) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}else if(texParam == REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}else if (texParam == MIRRORED_REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}else if(texParam == STRETCH_EDGE) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else if (texParam == STRETCH_EDGE) {
		float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}else {
		warning("class Texture.setParam(): Unknown param!\n");
	}

	glBindTexture(GL_TEXTURE_2D, prev_tex);
}
