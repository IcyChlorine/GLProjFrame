#version 330 core
in vec3 f_pos; // f for fragment
in vec3 f_norm;
in vec2 f_texcoord;

out vec4 FragColor;

uniform vec3 f_camera_pos;

uniform sampler2D tex_ambient;
uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;
uniform sampler2D tex_normal;
//uniform sampler2D tex_opacity;

// assume depth == gl_Position.z/gl_Position.w in [-1, 1] is the 
// unlinear depth: linear_depth(depth, ...)
// If use gl_FragCoord, whose z component is in [0, 1], normalize
// it first: linear_depth(gl_FragCoord.z*2-1, ...)
float linearize_depth(float depth, float zNear, float zFar)
{
	float d = -2*zNear*zFar / (depth*(zFar-zNear)-(zNear+zFar));
	return (d-zNear) / (zFar-zNear);
}

vec3 ambient_component(float fac_ambient, sampler2D tex_ambient, vec2 texcoord)
{
	return fac_ambient * texture(tex_ambient, texcoord).rgb;
}
vec3 diffuse_component(float fac_diffuse, 
					sampler2D tex_diffuse, vec2 texcoord, vec3 normal, vec3 light_dir)
{
	float direction_fac = dot(normalize(normal), normalize(light_dir));
	      direction_fac = max(direction_fac, 0.0);
	return fac_diffuse * direction_fac * texture(tex_diffuse, texcoord).rgb;
}
vec3 specular_component(float fac_specular, float shininess,
					sampler2D tex_specular, vec2 texcoord, 
					vec3 normal, vec3 light_dir, vec3 view_dir)
{
	vec3 reflect_dir = reflect(-light_dir, normal);
	float direction_fac = dot(normalize(view_dir), normalize(reflect_dir));
	      direction_fac = max(direction_fac, 0.0);
		  direction_fac = pow(direction_fac, shininess);
	return fac_specular * direction_fac * texture(tex_specular, texcoord).rgb;
}

void main() {
	vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 light_dir   = normalize(vec3(1.0, 1.0, 1.0));
	vec3 f_normal    = f_norm + texture(tex_normal, f_texcoord).xyz;

	//vec4 gray_color = vec4(0.2,0.2,0.2,1.0);

	// ambient
	FragColor = light_color * texture(tex_ambient, f_texcoord);

	// diffuse
	float diff_fac = max(dot(normalize(f_normal), normalize(light_dir)), 0);
	vec4 diff_color = texture(tex_diffuse, f_texcoord);
	FragColor += light_color * diff_fac * diff_color;
	
	// specular
	vec3 view_dir = normalize(f_camera_pos - f_pos);
	vec3 reflect_dir = reflect(-light_dir, f_normal);
	float spec_fac = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
	FragColor += light_color * spec_fac * texture(tex_specular, f_texcoord);
	
	FragColor.a = 1.0f;
	//FragColor.a = texture(tex_opacity, f_texcoord).r;
}