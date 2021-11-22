/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 09:46:35 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 21:07:19 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>

namespace wgl
{

Camera::Camera(float fovy, float aspect)
{
	this->position = glm::vec3(0.0, 0.0, 0.0);
	this->projection_matrix = glm::perspective(fovy, aspect, 0.1f, 1000.0f);
}
Camera::Camera() {}
Camera::~Camera()
{
	printf("destroyed camera\n");
}

glm::mat4x4 Camera::getViewMatrix()
{
	glm::mat4 view;
	view = glm::lookAt(position,
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));
	return (view);
}

glm::mat4x4 Camera::getProjectionMatrix()
{
	return (projection_matrix);
}

} // namespace wgl
