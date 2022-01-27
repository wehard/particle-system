#version 330 core

in float life;
in vec3 velocity;
out vec4 color;

uniform vec4 min_color;
uniform vec4 max_color;
uniform float max_life;

uniform int draw_normals;

void main()
{
	vec4 l_color = mix(min_color, max_color, life / max_life);
	vec3 n = normalize(abs(velocity));

	if (draw_normals > 0)
	{
		color = vec4(n, 1.0);
	}
	else
	{
		color = l_color;
	}

}
