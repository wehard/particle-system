#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_vel;
layout(location = 2) in float v_life;

uniform vec3 m_pos;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

out float distance;
out float life;
out vec3 velocity;


void main()
{
	life = v_life;
	velocity = v_vel;
	vec4 m = proj_matrix * view_matrix * model_matrix * vec4(m_pos, 1.0);
	vec4 p = proj_matrix * view_matrix * model_matrix * vec4(v_pos, 1.0);

	distance = 1.0 - length(m - p);

	gl_Position = p;
}
