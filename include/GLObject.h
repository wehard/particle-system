#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

class GLObject
{
private:
	GLuint vao;
	GLuint vbo;
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	GLObject(std::vector<float> vertexPositions);
	~GLObject();
	glm::mat4 getModelMatrix();
};


