#include "GLRenderer.h"

GLRenderer::GLRenderer()
{
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::Begin(Camera &camera)
{
	this->camera = &camera;
}

void GLRenderer::Draw(GLObject &object)
{
	glBindVertexArray(object.vao);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glDrawArrays(GL_TRIANGLES, 0, object.vertexCount);
}

void GLRenderer::Draw(GLObject &object, Shader &shader)
{
	shader.use();
	shader.setVec4("obj_color", object.color);
	shader.setMat4("proj_matrix", camera->getProjectionMatrix());
	shader.setMat4("view_matrix", camera->getViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());
	Draw(object);
}

void GLRenderer::DrawLines(GLObject &object, Shader &shader)
{
	shader.use();
	shader.setMat4("proj_matrix", camera->getProjectionMatrix());
	shader.setMat4("view_matrix", camera->getViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());

	glBindVertexArray(object.vao);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glDrawArrays(GL_LINES, 0, object.vertexCount);
}