#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec4 v_col;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in vec3 v_normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform vec4 obj_color;
uniform float b_scale;

out vec4 f_col;
out vec3 f_normal;
out vec4 o_col;

void main()
{
	o_col = v_col;
	f_col = v_col;
	f_normal = v_normal;
	mat4 model_view = view_matrix * model_matrix;
	gl_Position = proj_matrix * (model_view * vec4(0.0, 0.0, 0.0, 1.0) + vec4(v_pos.x * b_scale, v_pos.y * b_scale, 0.0, 0.0));
}
