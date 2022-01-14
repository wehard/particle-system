#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec4 v_col;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in vec3 v_normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform vec4 obj_color;

out vec4 f_col;
out vec3 f_normal;
out vec4 o_col;

void main()
{
	mat4 model = model_matrix;

	model[0][0] = 1.0;
	model[0][1] = 0.0;
	model[0][2] = 0.0;

	model[1][0] = 0.0;
	model[1][1] = 1.0;
	model[1][2] = 0.0;

	model[2][0] = 0.0;
	model[2][1] = 0.0;
	model[2][2] = 1.0;

	o_col = v_col;
	f_col = v_col;
	f_normal = v_normal;
	gl_Position = proj_matrix * view_matrix * model_matrix * vec4(v_pos, 1.0);
}
