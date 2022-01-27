#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_vel;
layout(location = 2) in float v_life;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;

out float life;
out vec3 velocity;


void main()
{
	life = v_life;
	velocity = v_vel;
	vec4 p = proj_matrix * view_matrix * vec4(v_pos, 1.0);


	gl_Position = p;
}
