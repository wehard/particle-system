#version 330 core
out vec4 col;

in vec2 f_uv;

uniform sampler2D screen_tex;
in vec4 gl_FragCoord;

void main()
{
	col = texture(screen_tex, f_uv);
}
