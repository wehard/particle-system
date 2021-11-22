/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 11:52:19 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 21:11:29 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "renderer.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include "text.h"

namespace wgl
{

Renderer::Renderer(Camera *camera) : camera(camera), lightPos(glm::vec3(0.0, 0.0, 50.0))
{
}

// Renderer::Renderer() : camera(nullptr)
// {
// }

Renderer::~Renderer()
{
	printf("destroyed renderer\n");
}

void Renderer::drawEntity(Entity *entity)
{
	entity->shader->use();
	entity->shader->setVec3("light_pos", camera->position);
	entity->shader->setVec3("light_color", glm::vec3(1.0, 1.0, 1.0));
	entity->shader->setMat4("view_matrix", camera->getViewMatrix());
	entity->shader->setMat4("proj_matrix", camera->getProjectionMatrix());
	entity->shader->setMat4("model_matrix", entity->getModelMatrix());
	entity->draw();
}

void Renderer::drawTexturedQuad(TexturedQuad *quad)
{
	if (!quad)
		return;
	quad->draw();
}


} // namespace wgl
