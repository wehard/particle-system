#version 330 core

in float life;
uniform vec4 min_color;
uniform vec4 max_color;
uniform float max_life;

out vec4 color;

void main()
{
	color = mix(min_color, max_color, life / max_life);
}
