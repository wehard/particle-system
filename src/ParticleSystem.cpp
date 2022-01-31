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

ParticleSystem::ParticleSystem(GLContext &gl, CLContext &cl, CLProgram &p, Shader &pShader, Shader &eShader) : gl(gl), cl(cl), clProgram(p), particleShader(pShader), emitterShader(eShader), initShape(SPHERE)
{
	minColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
	maxColor = glm::vec4(1.0, 0.0, 0.0, 0.0);

	CreateParticleBuffer();
	CreateGravityPointBuffer();
	CreateSeedBuffer();	

	InitParticles();
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

void ParticleSystem::InitParticles()
{
	useEmitter = false;
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = cl.queue;

	auto k = clProgram.GetKernel("init_particles");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &seedBuffer.clmem},
		{sizeof(GLint), &numParticles},
		{sizeof(t_init_shape), &initShape}};
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

	auto kernel = clProgram.GetKernel("init_particles_emitter");
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

void ParticleSystem::SetShape(t_init_shape shape)
{
	useEmitter = false;
	initShape = shape;
}

void ParticleSystem::Reset()
{
	if (useEmitter)
	{
		InitParticlesEmitter();
	}
	else
	{
		InitParticles();
	}
}

void ParticleSystem::UpdateGpBuffer(std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo)
{
	glBindBuffer(GL_ARRAY_BUFFER, gpBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_float4) * MAX_GP, (const GLvoid *)&gravityPoints[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::UpdateParticles(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo)
{
	cl_float4 clm;
	clm.s[0] = mouseInfo.world.x;
	clm.s[1] = mouseInfo.world.y;
	clm.s[2] = mouseInfo.world.z;
	clm.s[3] = 1.0f;

	bool mouseGravity = mouseInfo.gravity;
	float mouseMass = mouseInfo.mass;

	UpdateGpBuffer(gravityPoints, mouseInfo);
	int numGp = gravityPoints.size();
	auto kernel = clProgram.GetKernel("update_particles_gravity_points");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &gpBuffer.clmem},
		{sizeof(int), &numGp},
		{sizeof(cl_float4), &clm},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(bool), &mouseGravity},
		{sizeof(float), &mouseMass}
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

void ParticleSystem::UpdateParticlesEmitter(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo)
{
	cl_float4 clm = mouseInfo.CLTypeWorld();
	t_emitter e = emitter.CLType();

	UpdateGpBuffer(gravityPoints, mouseInfo);
	int numGp = gravityPoints.size();
	auto kernel = clProgram.GetKernel("update_particles_emitter");
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &pBuffer.clmem},
		{sizeof(cl_mem), &gpBuffer.clmem},
		{sizeof(cl_mem), &seedBuffer.clmem},
		{sizeof(int), &numGp},
		{sizeof(cl_float4), &clm},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(bool), &mouseInfo.gravity},
		{sizeof(float), &mouseInfo.mass},
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

void ParticleSystem::Update(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo)
{
	if (useEmitter)
	{
		this->UpdateParticlesEmitter(deltaTime, gravityPoints, mouseInfo);
	}
	else
	{
		this->UpdateParticles(deltaTime, gravityPoints, mouseInfo);
	}
}

void ParticleSystem::Render(Camera &camera, MouseInfo mouseInfo)
{
	if (useEmitter)
	{
		emitterShader.use();
		emitterShader.setMat4("proj_matrix", camera.getProjectionMatrix());
		emitterShader.setMat4("view_matrix", camera.getViewMatrix());
		emitterShader.setFloat("max_life", emitter.life);
		emitterShader.setVec4("min_color", minColor);
		emitterShader.setVec4("max_color", maxColor);
	}
	else
	{
		particleShader.use();
		particleShader.setVec4("min_color", glm::vec4(1.0, 0.0, 0.0, 0.1));
		particleShader.setVec4("max_color", glm::vec4(1.0, 1.0, 0.0, 0.1));
		particleShader.setMat4("proj_matrix", camera.getProjectionMatrix());
		particleShader.setMat4("view_matrix", camera.getViewMatrix());
		particleShader.setInt("draw_mouse", true);
		particleShader.setVec3("m_pos", mouseInfo.world);
	}

	glPointSize(particleSize);
	glBindVertexArray(pBuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer.vbo);
	glDrawArrays(GL_POINTS, 0, numParticles);
}