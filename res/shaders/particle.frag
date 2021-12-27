#version 330 core

in float distance;
in float speed;
out vec4 color;

void main()
{
	vec3 col = vec3(1.0, 1.0, 1.0) * pow(speed, 2.0);
	color = vec4(1.0,1.0,1.0, 1.0);
}
