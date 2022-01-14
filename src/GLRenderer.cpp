#include "GLRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

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

static glm::mat4 getModelMatrixBillboard(GLObject &object, glm::vec3 target)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), object.scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), object.position);
	glm::mat4 matRotate = glm::lookAt(object.position, target, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}


void GLRenderer::DrawBillboard(GLObject &object, Shader &shader)
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