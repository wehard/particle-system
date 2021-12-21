#version 330 core

in float distance;
out vec4 color;

void main()
{
	vec3 col = vec3(1.0, 1.0, 1.0) * pow(distance, 5.0);
	color = vec4(col, 1.0);
}
