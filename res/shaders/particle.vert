#version 330 core
layout(location = 0) in vec3 v_pos;

uniform vec2 m_pos;

out float distance;

void main()
{
	gl_Position = vec4(m_pos, 1.0, 1.0);
}
