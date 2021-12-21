/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:19:16 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/21 20:15:44 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParticleSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

static std::string loadKernelSource(std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

ParticleSystem::ParticleSystem(GLContext &gl, CLContext &cl) : gl(gl), cl(cl)
{
	shader = new glengine::Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	shader->setVec2("m_pos", glm::vec2(0.0, 0.0));

	cl::string src = loadKernelSource("./res/kernel/particles.cl").c_str();
	cl.addSource(src);
	cl.compileProgram();
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numParticles * 3, nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	try
	{
		clmem = clCreateFromGLBuffer(cl.ctx.get(), CL_MEM_READ_WRITE, vbo, nullptr);
	}
	catch (const cl::Error & e)
	{
		std::cout << "cl::Memory init error!: " << e.err() << std::endl;
	}
}

void ParticleSystem::init() 
{
	try
	{
		glFinish();
		// cl::CommandQueue &	queue;
		cl::Kernel			kernel(cl.program, "init_particles");

		kernel.setArg(0, &clmem);
		// kernel.setArg(1, sizeof(cl_int), &numParticles);

		// cl.queue.enqueueAcquireGLObjects(&clMem);
		// cl.queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numParticles), cl::NullRange);
		// cl.queue.enqueueReleaseGLObjects(&clMem);
		// cl.queue.finish();

		clEnqueueAcquireGLObjects(cl.queue.get(), 1, &clmem, 0, NULL, NULL);
		clEnqueueNDRangeKernel(cl.queue.get(), kernel.get(), 1, NULL, cl::NDRange(numParticles).get(), NULL, 0, NULL, NULL);
		clEnqueueReleaseGLObjects(cl.queue.get(), 1, &clmem, 0, nullptr, nullptr);
		clFinish(cl.queue.get());
	}
	catch (const cl::Error & e)
	{
		std::cout << e.err() << "\n" << e.what() << std::endl;
	}
}

void ParticleSystem::update(float deltaTime) 
{
	shader->use();
	shader->setVec2("m_pos", glm::vec2(0.0, 0.0));

	cl::Kernel	kernel(cl.program, "update_particles");
	kernel.setArg(0, &clmem);
	kernel.setArg(1, sizeof(cl_float), &deltaTime);

	glFinish();

	clEnqueueAcquireGLObjects(cl.queue.get(), 1, &clmem, 0, NULL, NULL);
	clEnqueueNDRangeKernel(cl.queue.get(), kernel.get(), 1, NULL, cl::NDRange(numParticles).get(), NULL, 0, NULL, NULL);
	clEnqueueReleaseGLObjects(cl.queue.get(), 1, &clmem, 0, nullptr, nullptr);
	clFinish(cl.queue.get());
}

ParticleSystem::~ParticleSystem()
{
}
