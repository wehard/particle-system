#version 330 core

in float life;
in vec3 velocity;
in float m_dist;
out vec4 color;

uniform vec4 min_color;
uniform vec4 max_color;
uniform int draw_normals;

uniform int draw_mouse;

void main()
{
	vec3 n = normalize(abs(velocity));
	color = mix(mix(min_color, max_color, length(velocity) / 10000.0f), vec4(n, 0.9), 0.4f);
}
