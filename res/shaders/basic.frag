#version 330 core
in vec4 f_col;
in vec3 f_normal;
in vec4 o_col;

uniform vec4 obj_color;

out vec4 color;

void main()
{
	color = obj_color; //f_col * vec4(f_normal, 1.0);// * 1.0 - (gl_FragCoord.y / 720);
}
