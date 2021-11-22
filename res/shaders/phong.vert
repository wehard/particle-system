#version 330 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec4 v_col;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in vec3 v_normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform vec3 light_pos;

out	vec3 normal;
out	vec3 f_pos;
out vec3 l_pos;
out vec4 vert_col;

void main()
{
	gl_Position = proj_matrix * view_matrix * model_matrix * vec4(v_pos, 1.0);

	f_pos = vec3(view_matrix * model_matrix * vec4(v_pos, 1.0f));
	normal = mat3(transpose(inverse(view_matrix * model_matrix))) * v_normal;
	l_pos = vec3(view_matrix * vec4(light_pos, 1.0));
	vert_col = v_col;
}
