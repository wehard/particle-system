/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/27 18:09:56 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 21:01:03 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity.h"
#include <stdlib.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "obj_loader.h"
#include "mesh.h"
#include "collider.h"

namespace wgl
{

Entity::Entity() : shader(0), mesh(0), rotation(0.0) {}

Entity::Entity(Shader *shader, Mesh *mesh) : shader(shader), mesh(mesh), rotation(0.0), scale({0.0,0.0,0.0})
{
	this->collider = new Collider(mesh);
	genBuffers();
}

Entity::Entity(Mesh *mesh) : mesh(mesh) {}

Entity::~Entity()
{
	glDeleteBuffers(1, &vbo_id);
	glDeleteBuffers(1, &cb_id);
	glDeleteBuffers(1, &uvb_id);
	glDeleteBuffers(1, &nb_id);
	glDeleteBuffers(1, &ebo_id);

	glDeleteVertexArrays(1, &vao_id);
	printf("destroyed entity: %d\n", vbo_id);
	delete collider;
}

void Entity::genBuffers()
{
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	glGenBuffers(1, &vbo_id);
	glGenBuffers(1, &cb_id);
	glGenBuffers(1, &uvb_id);
	glGenBuffers(1, &nb_id);
	glGenBuffers(1, &ebo_id);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->vertices.size(), &mesh->vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cb_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->colors.size(), &mesh->colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, uvb_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->uvs.size(), &mesh->uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, nb_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->normals.size(), &mesh->normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices.size(), &mesh->indices[0], GL_STATIC_DRAW);

	// printf("vao: %d vbo: %d uvb: %d, ebo: %d\n", vao_id, vbo_id, uvb_id, ebo_id);
}

void Entity::update(float deltaTime) {}

void Entity::draw()
{
	glBindVertexArray(vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
}

glm::mat4 Entity::getModelMatrix()
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

} // namespace wgl
