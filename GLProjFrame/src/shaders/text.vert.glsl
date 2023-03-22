#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform float wnd_width;
uniform float wnd_height;
//uniform mat4 projection;

// map [0,1] -> [-1,1]
float biuni(float value) {
	return value*2-1;
}

void main()
{
    gl_Position = vec4(biuni(vertex.x/wnd_width), biuni(1-vertex.y/wnd_height), 0.0, 1.0);
    TexCoords = vec2(vertex.z, vertex.w);
} 