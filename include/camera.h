/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 09:47:00 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 17:20:00 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace wgl
{

class Camera
{
private:
	glm::vec3 rotation;
	float fov;
	glm::mat4x4 projection_matrix;

public:
	glm::vec3 position;
	Camera(float fovy, float aspect);
	Camera();
	~Camera();
	glm::mat4x4 getViewMatrix();
	glm::mat4x4 getProjectionMatrix();
};

} // namespace wgl