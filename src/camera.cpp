/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 09:46:35 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 17:20:03 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace wgl
{

Camera::Camera(float fovy, float aspect)
{
	this->position = glm::vec3(0.0, 0.0, 0.0);
	this->projection_matrix = glm::perspective(fovy, aspect, 0.1f, 1000.0f);
}
Camera::Camera() {}
Camera::~Camera() {}

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