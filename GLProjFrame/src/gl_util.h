#pragma once
#ifndef _GL_UTIL_H
#define _GL_UTIL_H

#include <exception>
#include <stack>
#include <vector>
using std::vector;

#include "glad/glad.h"
#include "logging.h"

extern std::stack<bool> depth_stack;
extern std::stack<bool> blend_stack;

inline void get_and_bind_vao(unsigned int * pVAO) {
	glGenVertexArrays(1, pVAO);
	glBindVertexArray(*pVAO);
}
inline void get_and_bind_vbo(unsigned int * pVBO) {
	glGenBuffers(1, pVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *pVBO);
}
inline void get_and_bind_ibo(unsigned int * pIBO) {
	glGenBuffers(1, pIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *pIBO);
}
// three in one!
inline void init_buffers(unsigned int * pVAO, unsigned int * pVBO, unsigned int * pIBO) {
	get_and_bind_vao(pVAO);
	get_and_bind_vbo(pVBO);
	if(pIBO) get_and_bind_ibo(pIBO);
}


/* for GL_TEXTURE_2D only */
inline void gen_and_bind_texture(unsigned int * pTex) {
	glGenTextures(1, pTex);
	glBindTexture(GL_TEXTURE_2D, *pTex);
}
/* mostly shortcuts */
inline void unbind_vao() {
	glBindVertexArray(0);
}
inline void unbind_vbo() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
inline void unbind_ibo() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
inline void unbind_texture() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* set texture param for the current bound GL_TEXTURE_2D texture */
inline void set_texture_param(GLenum param) {
	// these two are usually unused
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// these two are more changeable under different demands
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
}

inline int check_gl_err(bool agh_if_no_error=false) {
	GLenum gl_errno = glGetError();
	if(gl_errno!=GL_NO_ERROR) {
		errorf("OpenGL error occured: errno=%d\n", gl_errno);
		throw std::exception();
	}else if(agh_if_no_error) {
		info("No OpenGL error at checkpoint!\n");
	}
}

/*
	specify vert format
	Example: suppose your vert data format is [xyz|rgb|uv], with each value being a float
	         then call the function like this:
	declare_interleaving_vert_data_layout(vector<int>{3,3,2});
*/
void declare_interleaving_vert_data_layout(vector<int> format);

/* a wrapper that masks out parameters of glTexImage2D that are rarely used */
inline void simplifid_tex_image_func(
	const void* data, GLint format, GLsizei width, GLsizei height
) {
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		width,height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data
	);
}

void set_and_push_depth_test_status(bool enabled);

void pop_and_resume_depth_test_status();

void set_and_push_blend_status(bool enabled);

void pop_and_resume_blend_status();

#endif /* _GL_UTIL_H */