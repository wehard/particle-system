/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collider.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/01 12:45:34 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:29:15 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "collider.h"
#include "mesh.h"
#include "entity.h"
#include <stdio.h>

namespace wgl
{

Collider::Collider(Mesh *mesh) : m_is_enabled(true)
{
	this->b = Mesh::calculateBounds(mesh);
}

Collider::~Collider()
{
}

void Collider::enable()
{
	m_is_enabled = true;
}

void Collider::disable()
{
	m_is_enabled = false;
}

bool Collider::isEnabled()
{
	return (m_is_enabled);
}

Bounds transform_bounds(Bounds b, glm::vec3 p, glm::vec3 s)
{
	b.x_min = (b.x_min * s.x) + p.x;
	b.y_min = (b.y_min * s.y) + p.y;
	b.z_min = (b.z_min * s.z) + p.z;
	b.x_max = (b.x_max * s.x) + p.x;
	b.y_max = (b.y_max * s.y) + p.y;
	b.z_max = (b.z_max * s.z) + p.z;
	return (b);
}

bool Collider::checkCollision(Entity *e1, Entity *e2, HitInfo *h, int ignore_mask)
{
	if (!e1 || !e2 || !e1->collider || !e2->collider || !e1->collider->isEnabled() || !e2->collider->isEnabled())
		return (false);
	Bounds b1 = transform_bounds(e1->collider->b, e1->position, e1->scale);
	Bounds b2 = transform_bounds(e2->collider->b, e2->position, e2->scale);

	const glm::vec3 face_normals[6] = {
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(-1.0, 0.0, 0.0),
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(0.0, -1.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0),
		glm::vec3(0.0, 0.0, -1.0)};
	float distance[6] = {
		(b2.x_max - b1.x_min),
		(b1.x_max - b2.x_min),
		(b2.y_max - b1.y_min),
		(b1.y_max - b2.y_min),
		(b2.z_max - b1.z_min),
		(b1.z_max - b2.z_min),
	};
	float min_dist = INFINITY;
	for (int i = 0; i < 6; i++)
	{
		if (distance[i] < 0.0)
			return (false);
		if (ignore_mask & (1 << i))
		{
			printf("ignoring face %d\n", i);
			continue;
		}
		if (i == 0 || distance[i] < min_dist)
		{
			min_dist = distance[i];
			h->normal = face_normals[i];
			h->faces = (1 << i);
		}
	}
	h->e = e2;
	return (true);
}

} // namespace wgl
