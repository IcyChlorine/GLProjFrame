#version 330 core
in vec3 f_pos; // f for fragment
in vec3 f_norm;
in vec2 f_texcoord;

out vec4 FragColor;

uniform float fac_ambient;
uniform float fac_diffuse;
uniform float fac_specular;
uniform float shininess;

//uniform vec3 environment_color = clear color = 0.2f, 0.3f, 0.3f
uniform vec3 f_camera_pos;

uniform sampler2D tex_ambient;
uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;
uniform sampler2D tex_normal;
uniform sampler2D tex_opacity;

uniform int flags;

// assume depth == gl_Position.z/gl_Position.w in [-1, 1] is the 
// unlinear depth: linear_depth(depth, ...)
// If use gl_FragCoord, whose z component is in [0, 1], normalize
// it first: linear_depth(gl_FragCoord.z*2-1, ...)
float linearize_depth(float depth, float zNear, float zFar)
{
	float d = -2*zNear*zFar / (depth*(zFar-zNear)-(zNear+zFar));
	return (d-zNear) / (zFar-zNear);
}

vec3 ambient_component(float fac_ambient, vec3 ambient_color)
{
	return fac_ambient * ambient_color;
}
vec3 diffuse_component(float fac_diffuse, vec3 diffuse_color,
						vec3 normal, vec3 light_dir)
{
	float direction_fac = dot(normalize(normal), normalize(light_dir));
	      direction_fac = max(direction_fac, 0.0);
	/* A trick for rendering translucent objects(wheat, fire, etc.) in mc map
	if((flags & 0x1) != 0) {
		direction_fac = abs(direction_fac);
	} else {
		direction_fac = max(direction_fac, 0.0);
	}*/
	return fac_diffuse * direction_fac * diffuse_color;
}
vec3 specular_component(float fac_specular, float shininess,
						vec3 specular_color, 
						vec3 normal, vec3 light_dir, vec3 view_dir)
{
	// shiniess += eps to avoid NaN arise from 0^0 - this is done by Material
	// to avoid redundant calculations
	vec3 reflect_dir = reflect(-light_dir, normal);
	float direction_fac = dot(normalize(view_dir), normalize(reflect_dir));
	      direction_fac = max(direction_fac, 0.0);
		  direction_fac = pow(direction_fac, shininess);
	return fac_specular * direction_fac * specular_color;
}

void main() {
	vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 light_dir   = normalize(vec3(1.0, 1.0, 1.0));
	vec3 f_normal    = f_norm + texture(tex_normal, f_texcoord).xyz;

	//vec4 gray_color = vec4(0.2,0.2,0.2,1.0);

	// ambient
	FragColor.rgb = ambient_component(fac_ambient, texture(tex_ambient, f_texcoord).rgb);
	//FragColor = vec4(0);

	// diffuse
	//float diff_fac = max(dot(normalize(f_normal), normalize(light_dir)), 0);
	//vec4 diff_color = texture(tex_diffuse, f_texcoord);
	//FragColor += light_color * diff_fac * diff_color;
	FragColor.rgb += diffuse_component(fac_diffuse, texture(tex_diffuse, f_texcoord).rgb, f_normal, light_dir);
	
	// specular
	//vec3 view_dir = normalize(f_camera_pos - f_pos);
	//vec3 reflect_dir = reflect(-light_dir, f_normal);
	//float spec_fac = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
	//FragColor += light_color * spec_fac * texture(tex_specular, f_texcoord);
	vec3 view_dir = normalize(f_camera_pos - f_pos);
	FragColor.rgb += specular_component(fac_specular, shininess, 
									texture(tex_specular, f_texcoord).rgb,
									f_normal, light_dir, view_dir); 
	
	// 0x1 == USE_OPACITY_TEXTURE
	if((flags & 0x1) != 0){ // use opacity texture
		FragColor.a = texture(tex_opacity, f_texcoord).r;
		if(FragColor.a==0.0f) discard;
	}
	else
		FragColor.a = 1.0f;
}