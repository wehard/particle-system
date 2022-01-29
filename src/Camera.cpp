/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 09:46:35 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/31 13:21:51 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : forward(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	float fovy = 45.0;
	float aspect = 1.777;
	this->projection_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->initPosition = position;
	update();
}

Camera::~Camera() {}

glm::mat4x4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

glm::mat4x4 Camera::getProjectionMatrix()
{
	return (projection_matrix);
}

void Camera::Move(CameraDirection direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		position += forward * velocity;
	if (direction == BACKWARD)
		position -= forward * velocity;
	if (direction == LEFT)
		position -= right * velocity;
	if (direction == RIGHT)
		position += right * velocity;
}

void Camera::Rotate(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch -= yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	update();
}

void Camera::update()
{
	glm::vec3 temp;
	temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	temp.y = sin(glm::radians(pitch));
	temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(temp);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}

void Camera::Reset(glm::vec3 position, float yaw, float pitch)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	update();
}