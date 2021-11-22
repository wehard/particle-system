/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 11:51:21 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 21:11:20 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core.h"
#include <vector>
#include "entity.h"
#include "shader.h"
#include "camera.h"
#include "textured_quad.h"
#include "text.h"
#include <memory>

namespace wgl
{

class Renderer
{
private:
	Camera *camera;
	glm::vec3 lightPos;

public:
	// Renderer();
	Renderer(Camera *camera);
	~Renderer();
	void drawEntity(Entity *entity);
	void drawTexturedQuad(TexturedQuad *quad);
	void drawText(Text *text);
};

} // namespace wgl
