#version 330 core

in float distance;
in vec3 velocity;
out vec4 color;

#define	COLOR_MAX	vec4(1, 0, 0, 0.9)
#define	COLOR_MIN	vec4(1.0, 1.0, 0.0, 0.9)

void main()
{
	color = mix(COLOR_MIN, COLOR_MAX, distance);
	// vec3 absvel = abs(velocity);
	// color = vec4(absvel*10f, 1.0f);
}
