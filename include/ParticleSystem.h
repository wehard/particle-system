/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/27 23:53:14 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

class ParticleSystem
{
private:
	cl_mem	clmem;
	CLContext &cl;
	GLContext &gl;
public:
	const size_t numParticles = 3000000;
	glm::vec2	m_pos;
	GLuint vao;
	GLuint vbo;
	glengine::Shader *shader;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init();
	void update(float deltaTime);
	~ParticleSystem();
};

