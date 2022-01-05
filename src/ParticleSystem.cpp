/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:19:16 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/05 00:29:03 by wkorande         ###   ########.fr       */
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
	float div = RAND_MAX / range;
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

void ParticleSystem::init()
{
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = clCtx.queue;

	cl_kernel kernel = clProgram->CreateKernel("init_particles_sphere");

	result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clmem);
	CLContext::CheckCLResult(result, "clSetKernelArg");
	result = clSetKernelArg(kernel, 1, sizeof(GLint), &numParticles);
	CLContext::CheckCLResult(result, "clSetKernelArg");

	glFinish();
	result = clEnqueueAcquireGLObjects(queue, 1, &clmem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueAcquireGLObjects");
	result = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numParticles, NULL, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueNDRangeKernel");
	result = clEnqueueReleaseGLObjects(queue, 1, &clmem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueReleaseGLObjects");
	result = clFinish(queue);
	CLContext::CheckCLResult(result, "clFinish");
}

void ParticleSystem::reset()
{
	init();
}

void ParticleSystem::update(float deltaTime)
{
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = clCtx.queue;

	cl_kernel kernel = clProgram->CreateKernel("update_particles");

	result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clmem);
	CLContext::CheckCLResult(result, "clSetKernelArg");

	result = clSetKernelArg(kernel, 1, sizeof(GLfloat), &deltaTime);
	CLContext::CheckCLResult(result, "clSetKernelArg");

	// Mouse pos
	result = clSetKernelArg(kernel, 2, sizeof(GLfloat), &m_pos.x);
	CLContext::CheckCLResult(result, "clSetKernelArg");
	result = clSetKernelArg(kernel, 3, sizeof(GLfloat), &m_pos.y);
	CLContext::CheckCLResult(result, "clSetKernelArg");

	glFinish();
	result = clEnqueueAcquireGLObjects(queue, 1, &clmem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueAcquireGLObjects");
	result = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numParticles, NULL, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueNDRangeKernel");
	result = clEnqueueReleaseGLObjects(queue, 1, &clmem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueReleaseGLObjects");
	result = clFinish(queue);
	CLContext::CheckCLResult(result, "clFinish");
}

ParticleSystem::~ParticleSystem()
{
}
