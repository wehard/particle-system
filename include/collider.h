/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collider.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/01 12:45:13 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:29:04 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "core.h"
#include "bounds.h"
#include <glm/vec3.hpp>

namespace wgl
{

class Mesh;
class Entity;

#define FACE_RIGHT (1 << 0)
#define FACE_LEFT (1 << 1)
#define FACE_TOP (1 << 2)
#define FACE_BOTTOM (1 << 3)
#define FACE_FRONT (1 << 4)
#define FACE_BACK (1 << 5)

struct HitInfo
{
	Entity *e;
	glm::vec3 normal;
	int faces = 0;
};

class Collider
{
private:
	Bounds b;
	bool m_is_enabled;

public:
	Collider(Mesh *mesh);
	~Collider();
	void enable();
	void disable();
	bool isEnabled();
	static bool checkCollision(Entity *e1, Entity *e2, HitInfo *h, int ignore_mask = 0);
};

} // namespace wgl
