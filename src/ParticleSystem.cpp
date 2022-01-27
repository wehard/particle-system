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
#include "GUIContext.h"
#include "GLRenderer.h"
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

#include <random>

#include "Shader.h"

static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

ParticleSystem::ParticleSystem(GLContext &gl, CLContext &cl) : gl(gl), cl(cl)
{
	clProgram = new CLProgram(this->cl, "./res/kernel/particles.cl");

	basicShader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	vertexColorShader = new Shader("res/shaders/vertex_color.vert", "res/shaders/vertex_color.frag");
	billboardShader = new Shader("res/shaders/billboard.vert", "res/shaders/billboard.frag");
	particleShader = new Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");

	minColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
	maxColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

	camera = Camera(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0));
	CreateParticleBuffer();
	CreateGravityPointBuffer();
	CreateSeedBuffer();	
}

void ParticleSystem::CreateSeedBuffer()
{
	cl_int result = CL_SUCCESS;


	std::default_random_engine generator;
	std::uniform_int_distribution<u_long> distribution(0, ULONG_MAX);

	u_long *hostseed = (u_long*)malloc(sizeof(u_long) * numParticles);
	for (size_t i = 0; i < numParticles; i++)
	{
		hostseed[i] = distribution(generator);
	}
	
	seedBuffer.clmem = clCreateBuffer(cl.ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numParticles * 8, hostseed, &result);
	free(hostseed);

	CLContext::CheckCLResult(result, "CreateSeedBuffer::clCreateBuffer");
}

ParticleSystem::~ParticleSystem()
{
	delete particleShader;
	delete basicShader;
	delete vertexColorShader;
	delete billboardShader;

	glDeleteBuffers(1, &pBuffer.vbo);
	glDeleteVertexArrays(1, &pBuffer.vao);
	glDeleteBuffers(1, &gpBuffer.vbo);
	glDeleteVertexArrays(1, &gpBuffer.vao);

	CLContext::CheckCLResult(clReleaseMemObject(pBuffer.clmem), "clReleaseMemObject::pBuffer");
	CLContext::CheckCLResult(clReleaseMemObject(gpBuffer.clmem), "clReleaseMemObject::gpBuffer");
	CLContext::CheckCLResult(clReleaseMemObject(seedBuffer.clmem), "clReleaseMemObject::seedBuffer");
}

void ParticleSystem::CreateParticleBuffer()
{
	cl_int result = CL_SUCCESS;

	glGenVertexArrays(1, &pBuffer.vao);
	glBindVertexArray(pBuffer.vao);
	glGenBuffers(1, &pBuffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t_particle) * numParticles, (const GLvoid *)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), (const GLvoid *)16);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(t_particle), (const GLvoid *)32);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	pBuffer.clmem = clCreateFromGLBuffer(cl.ctx, CL_MEM_READ_WRITE, pBuffer.vbo, &result);
	CLContext::CheckCLResult(result, "ParticleSystem::createParticleBuffer");
}

void ParticleSystem::CreateGravityPointBuffer()
{
	cl_int result = CL_SUCCESS;
	
	glGenVertexArrays(1, &gpBuffer.vao);
	glBindVertexArray(gpBuffer.vao);
	glGenBuffers(1, &gpBuffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gpBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_float4) * MAX_GP, (const GLvoid *)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cl_float4), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	gpBuffer.clmem = clCreateFromGLBuffer(cl.ctx, CL_MEM_READ_WRITE, gpBuffer.vbo, &result);
	CLContext::CheckCLResult(result, "ParticleSystem::createGravityPointBuffer");
}

void ParticleSystem::AddGravityPoint()
{
	if (this->gravityPoints.size() < MAX_GP)
	{
		cl_float4 f;
		f.s[0] = this->mouseInfo.world.x;
		f.s[1] = this->mouseInfo.world.y;
		f.s[2] = this->mouseInfo.world.z;
		f.s[3] = 1.0;
		this->gravityPoints.push_back(f);
	}
}

void ParticleSystem::InitParticles(t_init_shape shape)
{
	useEmitter = false;
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = cl.queue;

	auto k = clProgram->GetKernel("init_particles");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &seedBuffer.clmem},
		{sizeof(GLint), &numParticles},
		{sizeof(t_init_shape), &shape}};
	k->SetArgs(args, 4);
	glFinish();
	cl.AquireGLObject(pBuffer.clmem);
	k->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(pBuffer.clmem);
	CLContext::CheckCLResult(clFinish(queue), "clFinish");
}

void ParticleSystem::InitParticlesEmitter()
{
	cl_int result = CL_SUCCESS;

	t_emitter e = emitter.CLType();

	auto kernel = clProgram->GetKernel("init_particles_emitter");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &seedBuffer.clmem},
		{sizeof(GLint), &numParticles},
		{sizeof(t_emitter), &e}};
	kernel->SetArgs(args, 4);
	glFinish();
	cl.AquireGLObject(pBuffer.clmem);
	kernel->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(pBuffer.clmem);
	CLContext::CheckCLResult(clFinish(cl.queue), "clFinish");
}

void ParticleSystem::Reset()
{
	InitParticles(RECT);
}

void ParticleSystem::UpdateGpBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, gpBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_float4) * MAX_GP, (const GLvoid *)&gravityPoints[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::UpdateParticles(float deltaTime)
{
	cl_float4 clm;
	clm.s[0] = this->mouseInfo.world.x;
	clm.s[1] = this->mouseInfo.world.y;
	clm.s[2] = this->mouseInfo.world.z;
	clm.s[3] = 1.0f;

	UpdateGpBuffer();
	int numGp = gravityPoints.size();
	auto kernel = clProgram->GetKernel("update_particles_gravity_points");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &gpBuffer.clmem},
		{sizeof(int), &numGp},
		{sizeof(cl_float4), &clm},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(int), &mouseGravity},
		{sizeof(float), &mouseGravityScale}
		};
	kernel->SetArgs(args, 7);
	glFinish();
	cl.AquireGLObject(pBuffer.clmem);
	cl.AquireGLObject(gpBuffer.clmem);
	kernel->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(pBuffer.clmem);
	cl.ReleaseGLObject(gpBuffer.clmem);
	CLContext::CheckCLResult(clFinish(cl.queue), "clFinish");
}

void ParticleSystem::UpdateParticlesEmitter(float deltaTime)
{
	cl_float4 clm = this->mouseInfo.CLTypeWorld();
	t_emitter e = emitter.CLType();

	UpdateGpBuffer();
	int numGp = gravityPoints.size();
	auto kernel = clProgram->GetKernel("update_particles_emitter");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &gpBuffer.clmem},
		{sizeof(cl_mem), &seedBuffer.clmem},
		{sizeof(int), &numGp},
		{sizeof(cl_float4), &clm},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(int), &mouseGravity},
		{sizeof(float), &mouseGravityScale},
		{sizeof(t_emitter), &e}};
	kernel->SetArgs(args, 9);
	glFinish();
	cl.AquireGLObject(pBuffer.clmem);
	cl.AquireGLObject(gpBuffer.clmem);
	kernel->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(pBuffer.clmem);
	cl.ReleaseGLObject(gpBuffer.clmem);
	CLContext::CheckCLResult(clFinish(cl.queue), "clFinish");
}

void ParticleSystem::Run()
{
	lastTime = glfwGetTime();
	double lastUpdateFpsTime = lastTime;
	int frameCount = 0;

	auto renderer = GLRenderer();
	GUIContext gui;
	gui.Init(gl.window, gl.glslVersion);

	auto plane = GLObject::Plane();
	auto gp = GLObject::Triangle();
	gp.color = glm::vec4(0.0, 0.8, 0.8, 1.0);
	gp.scale = glm::vec3(0.05);

	auto wAxis = GLObject::Axis();

	auto eStar = GLObject::Star();
	eStar.scale = glm::vec3(0.05);

	auto grid = GLObject::Grid(20, 20);
	grid.scale = glm::vec3(2.0); // TODO: Needs to be done during vertex creation


	auto eAxis = GLObject::Axis();

	glfwSetWindowUserPointer(gl.window, this);

	while (!glfwWindowShouldClose(gl.window) && glfwGetKey(gl.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;

		if (currentTime - lastUpdateFpsTime > 1.0)
		{
			fps = frameCount / lastUpdateFpsTime;
			lastUpdateFpsTime = currentTime;
		}

		if (glfwGetKey(gl.window, GLFW_KEY_W))
		{
			camera.Move(FORWARD, deltaTime);
		}
		if (glfwGetKey(gl.window, GLFW_KEY_S))
		{
			camera.Move(BACKWARD, deltaTime);
		}
		if (glfwGetKey(gl.window, GLFW_KEY_A))
		{
			camera.Move(LEFT, deltaTime);
		}
		if (glfwGetKey(gl.window, GLFW_KEY_D))
		{
			camera.Move(RIGHT, deltaTime);
		}

		if (useEmitter)
		{
			this->UpdateParticlesEmitter(deltaTime);
		}
		else
			this->UpdateParticles(deltaTime);

		gui.Update(*this);

		// Render here!
		renderer.Begin(camera);
		glClearColor(gl.clearColor.x, gl.clearColor.y, gl.clearColor.z, gl.clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		particleShader->use();
		particleShader->setFloat("max_life", emitter.life);
		particleShader->setVec3("m_pos", mouseInfo.world);
		particleShader->setVec4("min_color", minColor);
		particleShader->setVec4("max_color", maxColor);
		particleShader->setMat4("proj_matrix", camera.getProjectionMatrix());
		particleShader->setMat4("view_matrix", camera.getViewMatrix());
		particleShader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0), glm::vec3(1.0)));

		glPointSize(particleSize);
		glBindVertexArray(pBuffer.vao);
		glBindBuffer(GL_ARRAY_BUFFER, pBuffer.vbo);
		glDrawArrays(GL_POINTS, 0, numParticles);

		if (showOverlays)
		{
			if (gravityPoints.size() > 0)
			{
				for (auto a : gravityPoints)
				{
					gp.position = glm::vec3(a.s[0], a.s[1], a.s[2]);
					renderer.DrawBillboard(gp, 0.05, *billboardShader);
				}
			}

			if (mouseGravity)
			{
				gp.position = mouseInfo.world;
				renderer.DrawBillboard(gp, 0.05 + (mouseGravityScale * 0.001), *billboardShader);
			}

			renderer.DrawLines(wAxis, *vertexColorShader);

			if (useEmitter)
			{
				eStar.position = emitter.position;
				renderer.DrawBillboard(eStar, 0.05, *billboardShader);
				eAxis.position = emitter.position;
				eAxis.rotation = emitter.direction;
				renderer.DrawLines(eAxis, *vertexColorShader);
			}

			basicShader->use();
			basicShader->setVec4("obj_color", grid.color);
			renderer.DrawLines(grid, *basicShader);
		}

		gui.Render();

		glfwSwapBuffers(gl.window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
	gui.Shutdown();
}
