/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:19:16 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/06 14:47:20 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParticleSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

static float randfrom(float min, float max)
{
	float range = (max - min);
	float div = (float)RAND_MAX / range;
	return min + (rand() / div);
}

static std::string loadKernelSource(std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

ParticleSystem::ParticleSystem(GLContext &glCtx, CLContext &clCtx) : glCtx(glCtx), clCtx(clCtx)
{
	shader = new glengine::Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	shader->setVec2("m_pos", glm::vec2(0.5, 0.0));
	
	clProgram = new CLProgram(this->clCtx, "./res/kernel/particles.cl");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// positions
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t_particle) * numParticles, (const GLvoid *)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), (const GLvoid *)16);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	cl_int result = CL_SUCCESS;
	clmem = clCreateFromGLBuffer(clCtx.ctx, CL_MEM_READ_WRITE, vbo, nullptr);
	CLContext::CheckCLResult(result, "clCreateFromGLBuffer");
}

void ParticleSystem::init(const char *initKernel)
{
	this->rotation = glm::vec3(0.0);
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = clCtx.queue;

	auto k = clProgram->GetKernel(initKernel);
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &clmem},
		{sizeof(GLint), &numParticles}
	};
	k->SetArgs(args, 2);
	glFinish();
	clCtx.AquireGLObject(clmem);
	k->Enqueue(clCtx.queue, numParticles);
	clCtx.ReleaseGLObject(clmem);
	CLContext::CheckCLResult(clFinish(queue), "clFinish");
}

void ParticleSystem::reset()
{
	init("init_particles_cube");
}

void ParticleSystem::update(float deltaTime)
{
	auto kernel = clProgram->GetKernel("update_particles_test");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &clmem},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(GLfloat), &mouseInfo.world.x},
		{sizeof(GLfloat), &mouseInfo.world.y}
	};
	kernel->SetArgs(args, 4);
	glFinish();
	clCtx.AquireGLObject(clmem);
	kernel->Enqueue(clCtx.queue, numParticles);
	clCtx.ReleaseGLObject(clmem);
	CLContext::CheckCLResult(clFinish(clCtx.queue), "clFinish");
}

ParticleSystem::~ParticleSystem()
{
}
