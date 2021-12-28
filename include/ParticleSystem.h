/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/28 16:20:41 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

typedef struct
{
	cl_float4 pos;
	cl_float4 vel;
}		t_particle;

class ParticleSystem
{
private:
	cl_mem	clmem;
	CLContext &cl;
	GLContext &gl;
public:
	const size_t numParticles = 2000000;
	glm::vec2	m_pos;
	GLuint vao;
	GLuint vbo;
	glengine::Shader *shader;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init();
	void update(float deltaTime);
	~ParticleSystem();
};

