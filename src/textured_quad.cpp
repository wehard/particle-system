#include "textured_quad.h"

#include <GL/glew.h>
#include <stdio.h>
#include "texture.h"

namespace wgl
{

TexturedQuad::TexturedQuad(Texture *texture) : texture(texture)
{
	vertices = {
		-1.0f,
		-1.0f,
		0.0f,
		-1.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		-1.0f,
		0.0f,
	};

	uvs = {
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		0.0f,
	};

	indices = {
		0,
		1,
		2,
		0,
		2,
		3,
	};

	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	glGenBuffers(1, &vbo_id);
	glGenBuffers(1, &uvb_id);
	glGenBuffers(1, &ebo_id);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvb_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvs.size(), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	// printf("vao: %d vbo: %d uvb: %d, ebo: %d\n", vao_id, vbo_id, uvb_id, ebo_id);
}

TexturedQuad::~TexturedQuad() {}

void TexturedQuad::draw()
{
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glBindVertexArray(vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace wgl