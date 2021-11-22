#version 330 core
out vec4 out_col;

in vec2 f_uv;

uniform sampler2D font_texture;
uniform vec3 object_color;

void main()
{
	vec4 c = texture(font_texture, f_uv);
	if (c.r < 0.8)
		discard;
	out_col = vec4(object_color, 1.0) * c;
}
