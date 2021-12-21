#version 330 core
layout(location = 0) in vec3 v_pos;

uniform vec2 m_pos;

out float distance;

void main()
{
	distance = 1.0 - length(vec2(m_pos.x, m_pos.y) - vec2(v_pos.x, v_pos.y));
	gl_Position = vec4(v_pos, 1.0);
}
