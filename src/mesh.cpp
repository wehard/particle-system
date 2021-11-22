/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/31 11:01:04 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:29:28 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mesh.h"
#include "bounds.h"
#include <stdio.h>
#include <math.h>
#include <glm/vec3.hpp>

namespace wgl
{

Mesh::Mesh(/* args */)
{
}

Mesh::~Mesh()
{
	// clear();
	printf("destroyed mesh: %s\n", name.c_str());
}

Mesh *Mesh::makeQuad()
{
	Mesh *m = new Mesh();
	m->vertices = {
		-0.5f,
		-0.5f,
		0.0f,
		-0.5f,
		0.5f,
		0.0f,
		0.5f,
		0.5f,
		0.0f,
		0.5f,
		-0.5f,
		0.0f,
	};

	m->colors = {
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
	};

	m->uvs = {
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		0.0f,
	};

	m->indices = {
		0,
		1,
		2,
		2,
		3,
		0,
	};
	return (m);
}

void Mesh::setVertexColors(glm::vec4 c)
{
	for (size_t i = 0; i < colors.size(); i += 4)
	{
		colors[i] = c.r;
		colors[i + 1] = c.g;
		colors[i + 2] = c.b;
		colors[i + 3] = c.a;
	}
}

size_t Mesh::getNumVertices()
{
	return (vertices.size() / 3);
}

void Mesh::print(Mesh *m)
{
	printf("vertices: %zu\n", m->getNumVertices());
	for (size_t i = 0; i < m->vertices.size(); i += 3)
	{
		printf("v %.2f, %.2f, %.2f\tn %.2f, %.2f, %.2f\n",
			   m->vertices[i], m->vertices[i + 1], m->vertices[i + 2],
			   m->normals[i], m->normals[i + 1], m->normals[i + 2]);
	}
}

void Mesh::clear()
{
	vertices.clear();
	normals.clear();
	colors.clear();
	uvs.clear();
	indices.clear();
	printf("cleared mesh: %s\n", name.c_str());
}

Bounds Mesh::calculateBounds(Mesh *mesh)
{
	Bounds b = {
		b.x_min = INFINITY, b.x_max = -INFINITY,
		b.y_min = INFINITY, b.y_max = -INFINITY,
		b.z_min = INFINITY, b.z_max = -INFINITY};
	if (!mesh)
		return (b);
	for (size_t i = 0; i < mesh->vertices.size(); i += 3)
	{
		glm::vec3 cur = glm::vec3(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);
		if (cur.x < b.x_min)
			b.x_min = cur.x;
		if (cur.y < b.y_min)
			b.y_min = cur.y;
		if (cur.z < b.z_min)
			b.z_min = cur.z;
		if (cur.x > b.x_max)
			b.x_max = cur.x;
		if (cur.y > b.y_max)
			b.y_max = cur.y;
		if (cur.z > b.z_max)
			b.z_max = cur.z;
	}
	return (b);
}

} // namespace wgl
