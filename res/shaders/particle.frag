#version 330 core

in float life;
in float distance;
in vec3 velocity;
out vec4 color;

uniform vec4 min_color;
uniform vec4 max_color;
uniform float max_life;

#define	MAX_VEL 20000.0

void main()
{
	color = mix(min_color, max_color, life / max_life);
}
