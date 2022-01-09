#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
private:
	float fov;
	glm::mat4x4 projection_matrix;
	glm::vec3 target;
public:
	glm::vec3 rotation;
	glm::vec3 position;
	Camera(float fovy, float aspect);
	Camera();
	~Camera();
	glm::mat4x4 getViewMatrix();
	glm::mat4x4 getProjectionMatrix();
	void lookAt(glm::vec3 target);
};