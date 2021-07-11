#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;

out vec3 midColor;
out vec2 midTexCoord;

uniform mat4 transform;
uniform mat4 proj;

void main()
{
    gl_Position = proj * transform * vec4(inPos, 1.0f);
    midColor = inColor;//这两个原封不动传给frag shader
    midTexCoord = inTexCoord;
}