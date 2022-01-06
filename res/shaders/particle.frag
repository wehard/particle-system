#version 330 core

in float distance;
in vec3 velocity;
out vec4 color;

uniform vec4 min_color;
uniform vec4 max_color;

#define	COLOR_MAX	vec4(1, 0, 0, 1.0)
#define	COLOR_MIN	vec4(1.0, 1.0, 0.0, 1.0)

void main()
{
	color = mix(min_color, max_color, length(velocity) * 0.0001);
}
