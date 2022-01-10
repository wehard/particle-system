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
	shader.setVec4("obj_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.setMat4("proj_matrix", camera->getProjectionMatrix());
	shader.setMat4("view_matrix", camera->getViewMatrix());
	shader.setMat4("model_matrix", object.getModelMatrix());
	Draw(object);
}