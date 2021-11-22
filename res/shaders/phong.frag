#version 330 core

out vec4 color;

in vec3 f_pos;
in vec3 normal;
in vec3 l_pos;
in vec4 vert_col;

uniform vec3 light_color;


void main()
{
	// Ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * light_color;

	 // Diffuse
	vec3 norm = normalize(normal);
	vec3 l_dir = normalize(l_pos - f_pos);
	float diff = max(dot(norm, l_dir), 0.0);
	vec3 diffuse = diff * light_color;

	// Specular
	float spec_strength = 0.5f;
	vec3 view_dir = normalize(-f_pos);
	vec3 reflect_dir = reflect(-l_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = spec_strength * spec * light_color;

	vec3 result = (ambient + diffuse + specular) * vert_col.rgb;
	color = vec4(result, 1.0f);
}
