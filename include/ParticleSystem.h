/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/31 21:46:18 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

#include "CLProgram.h"

typedef struct
{
	cl_float4 pos;
	cl_float4 vel;
}		t_particle;

class ParticleSystem
{
private:
	cl_mem	clmem;
public:
	glm::vec3 rotation = glm::vec3(0.0);
	CLProgram *clProgram;
	CLContext &clCtx;
	GLContext &glCtx;
	const size_t numParticles = 2000000;
	glm::vec3	m_pos;
	GLuint vao;
	GLuint vbo;
	glengine::Shader *shader;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init(const char *initKernel);
	void update(float deltaTime);
	void reset();
	~ParticleSystem();
};

