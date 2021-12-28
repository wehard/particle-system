/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:19:16 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/28 15:04:41 by wkorande         ###   ########.fr       */
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

static void	checkCLSuccess(cl_int errNum, std::string name) {
	if (errNum != CL_SUCCESS) {
		std::cout << "OpenCL error " << errNum << " : " << name << std::endl;
		exit(EXIT_FAILURE);
	}
}

ParticleSystem::ParticleSystem(GLContext &gl, CLContext &cl) : gl(gl), cl(cl)
{
	shader = new glengine::Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	shader->setVec2("m_pos", glm::vec2(0.5, 0.0));


	// GLfloat particles[numParticles * 6];
	// for (size_t i = 0; i < numParticles * 6; i += 6)
	// {
	// 	particles[i+0] = randfrom(-1.0, 1.0);
	// 	particles[i+1] = randfrom(-1.0, 1.0);
	// 	particles[i+2] = 0.0;

	// 	particles[i+3] = randfrom(-1.0, 1.0);
	// 	particles[i+4] = randfrom(-1.0, 1.0);
	// 	particles[i+5] = 0.0;
	// }

	cl::string src = loadKernelSource("./res/kernel/particles.cl").c_str();
	cl.addSource(src);
	cl.compileProgram();
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// positions
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t_particle) * numParticles, (const GLvoid*)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
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
		// cl::Kernel			kernel(cl.program, "init_particles");
		cl_int result = CL_SUCCESS;
		cl_command_queue queue = cl.queue;
		cl_kernel kernel = clCreateKernel(cl.program.get(), "init_particles", &result);
		checkCLSuccess(result, "clCreateKernel");

		result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clmem);
		checkCLSuccess(result, "clSetKernelArg");
		result = clSetKernelArg(kernel, 1, sizeof(GLint), &numParticles);
		checkCLSuccess(result, "clSetKernelArg");


		result = clEnqueueAcquireGLObjects(queue, 1, &clmem, 0, NULL, NULL);
		checkCLSuccess(result, "clEnqueueAcquireGLObjects");
		result = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numParticles, NULL, 0, NULL, NULL);
		checkCLSuccess(result, "clEnqueueNDRangeKernel");
		result = clEnqueueReleaseGLObjects(queue, 1, &clmem, 0, nullptr, nullptr);
		// checkCLSuccess(result, "clEnqueueReleaseGLObjects");
		result = clFinish(queue);
		// checkCLSuccess(result, "clFinish");
	}
	catch (const cl::Error & e)
	{
		std::cout << e.err() << "\n" << e.what() << std::endl;
	}
}

void ParticleSystem::update(float deltaTime) 
{
	glFinish();
	// cl::CommandQueue &	queue;
	// cl::Kernel			kernel(cl.program, "init_particles");
	cl_int result = CL_SUCCESS;
	cl_kernel kernel = clCreateKernel(cl.program.get(), "update_particles", &result);
	cl_command_queue queue = cl.queue;
	checkCLSuccess(result, "clCreateKernel");

	result = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clmem);
	checkCLSuccess(result, "clSetKernelArg");

	result = clSetKernelArg(kernel, 1, sizeof(GLfloat), &deltaTime);
	checkCLSuccess(result, "clSetKernelArg");

	// Mouse pos
	result = clSetKernelArg(kernel, 2, sizeof(GLfloat), &m_pos.x);
	checkCLSuccess(result, "clSetKernelArg");
	result = clSetKernelArg(kernel, 3, sizeof(GLfloat), &m_pos.y);
	checkCLSuccess(result, "clSetKernelArg");

	result = clEnqueueAcquireGLObjects(queue, 1, &clmem, 0, NULL, NULL);
	checkCLSuccess(result, "clEnqueueAcquireGLObjects");
	result = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numParticles, NULL, 0, NULL, NULL);
	checkCLSuccess(result, "clEnqueueNDRangeKernel");
	result = clEnqueueReleaseGLObjects(queue, 1, &clmem, 0, nullptr, nullptr);
	// checkCLSuccess(result, "clEnqueueReleaseGLObjects");
	result = clFinish(queue);
	// checkCLSuccess(result, "clFinish");
}

ParticleSystem::~ParticleSystem()
{
}
