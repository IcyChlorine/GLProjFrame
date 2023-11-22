#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_norm;
layout (location = 2) in vec2 v_texcoord;
// the following two are not used so far
//layout (location = 3) in vec3 v_tan;
//layout (location = 4) in vec3 v_bitan;

out vec3 f_pos;
out vec3 f_norm;
out vec2 f_texcoord;

uniform mat4 transform;
uniform mat4 proj;

void main() {
	vec4 tmp = transform * vec4(v_pos, 1.0);
	f_pos = tmp.xyz;
	gl_Position = proj * tmp;
	f_norm = v_norm;
	f_texcoord = v_texcoord;
}