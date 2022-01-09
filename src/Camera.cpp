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

Camera::Camera(float fovy, float aspect)
{
	this->position = glm::vec3(0.0, 0.0, 0.0);
	this->projection_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	this->target = glm::vec3(0.0, 0.0, -1.0);
}

Camera::Camera() {}

Camera::~Camera()
{
	printf("destroyed camera\n");
}

glm::mat4x4 Camera::getViewMatrix()
{
	glm::mat4 view;
	glm::vec3 forward = position + glm::vec3(0.0, 0.0, -1.0);
	view = glm::lookAt(position,
					   forward,
					   glm::vec3(0.0f, 1.0f, 0.0f));
	return (view);
}

glm::mat4x4 Camera::getProjectionMatrix()
{
	return (projection_matrix);
}

void Camera::lookAt(glm::vec3 target)
{
	this->target = target;
}
