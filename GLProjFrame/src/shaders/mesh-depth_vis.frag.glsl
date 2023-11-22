#version 330 core
in vec3 f_norm;
in vec2 f_texcoord;
in vec4 probe;//==gl_Position

out vec4 FragColor;

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

// assume depth == gl_Position.z/gl_Position.w in [-1, 1] is the 
// unlinear depth: linear_depth(depth, ...)
// If use gl_FragCoord, whose z component is in [0, 1], normalize
// it first: linear_depth(gl_FragCoord.z*2-1, ...)
float linearize_depth(float depth,float zNear,float zFar)
{
	float d = -2*zNear*zFar / (depth*(zFar-zNear)-(zNear+zFar));
	return (d-zNear) / (zFar-zNear);
}

void main() {
	//FragColor = vec4(f_norm,1.0); //texture(diffuse,f_texcoord);
	float t;
	t = probe.z/probe.w;
	if(gl_FragCoord.x>400) t = 2*gl_FragCoord.z-1;
	float normalized_depth = linearize_depth(t, 0.1, 100.0);
	FragColor = vec4(vec3(
		1-6*pow(normalized_depth, 1)
	), 1.0);
	
	//FragColor = vec4(vec3(gl_FragCoord.w)/2,1.0);

	
}