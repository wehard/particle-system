/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:19:16 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/20 00:16:40 by wkorande         ###   ########.fr       */
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
		clMem.push_back(cl::BufferGL(cl.ctx, CL_MEM_READ_WRITE, vbo, nullptr));
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
		cl::CommandQueue &	queue = cl.queue;
		cl::Kernel			kernel(cl.program, "init_particles");

		kernel.setArg(0, clMem);
		// kernel.setArg(1, sizeof(cl_int), &numParticles);

		queue.enqueueAcquireGLObjects(&clMem);
		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numParticles), cl::NullRange);
		queue.enqueueReleaseGLObjects(&clMem);
		queue.finish();
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

	cl::CommandQueue &	queue = cl.queue;

	cl::Kernel	kernel(cl.program, "update_particles");
	kernel.setArg(0, &clMem);
	kernel.setArg(1, sizeof(cl_float), &deltaTime);

	glFinish();

	queue.enqueueAcquireGLObjects(&clMem);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numParticles), cl::NullRange);
	cl_int result = queue.finish();
	if (result != CL_SUCCESS)
	{
		std::cout << result << std::endl;
	}
	queue.enqueueReleaseGLObjects(&clMem);
	queue.finish();
}

ParticleSystem::~ParticleSystem()
{
}
