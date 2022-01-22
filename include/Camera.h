#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 100.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum CameraDirection
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
private:
	float fov;
	glm::mat4x4 projection_matrix;
	void update();

public:
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// euler Angles
	float yaw;
	float pitch;
	// camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	~Camera();
	glm::mat4x4 getViewMatrix();
	glm::mat4x4 getProjectionMatrix();
	void Move(CameraDirection direction, float deltaTime);
	void Rotate(float xoffset, float yoffset, bool constrainPitch = true);
};