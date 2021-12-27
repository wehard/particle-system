#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_vel;

uniform vec2 m_pos;

out float distance;
out vec3 velocity;

void main()
{
	velocity = v_vel;
	distance = 1.0 - length(vec2(m_pos.x, m_pos.y) - vec2(v_pos.x, v_pos.y));
	gl_Position = vec4(v_pos, 1.0);
}
