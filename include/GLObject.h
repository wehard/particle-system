#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

class GLObject
{
private:
public:
	GLsizei vertexCount;
	GLuint vao;
	GLuint vbo;
	GLuint cbo;
	glm::vec3 position = glm::vec3(0.0);
	glm::vec3 rotation = glm::vec3(0.0);
	glm::vec3 scale = glm::vec3(1.0);
	glm::vec4 color = glm::vec4(1.0);
	GLObject(std::vector<float> vertexPositions);
	GLObject(std::vector<float> vertexPositions, std::vector<float> vertexColors);
	~GLObject();
	glm::mat4 getModelMatrix();
	static GLObject Plane();
	static GLObject Triangle();
	static GLObject Axis();
};


