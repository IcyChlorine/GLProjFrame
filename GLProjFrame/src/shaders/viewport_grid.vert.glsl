#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
out vec4 pos_clip; // clip space coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 tmp = model * vec4(aPos, 1.0f);
    pos = tmp.xyz;
    gl_Position = proj * view * tmp;
    pos_clip = gl_Position;
}