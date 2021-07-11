#version 330 core
out vec4 FragColor;
  
in vec3 midColor;
in vec2 midTexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, midTexCoord) * vec4(midColor, 1.0);
}