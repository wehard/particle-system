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
#include "Shader.h"
#include "GLRenderer.h"
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

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
	particleShader = new Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	particleShader->setVec2("m_pos", glm::vec2(0.5, 0.0));

	clProgram = new CLProgram(this->cl, "./res/kernel/particles.cl");

	minColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
	maxColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

	CreateParticleBuffer();
	CreateGravityPointBuffer();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::InitParticles(const char *initKernel)
{
	cl_int result = CL_SUCCESS;
	cl_command_queue queue = cl.queue;

	auto k = clProgram->GetKernel(initKernel);
	std::vector<CLKernelArg> args = {
		{sizeof(cl_mem), &clmem},
		{sizeof(GLint), &numParticles}};
	k->SetArgs(args, 2);
	glFinish();
	cl.AquireGLObject(clmem);
	k->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(clmem);
	CLContext::CheckCLResult(clFinish(queue), "clFinish");
}

void ParticleSystem::CreateParticleBuffer()
{
	glGenVertexArrays(1, &pBuffer.vao);
	glBindVertexArray(pBuffer.vao);
	glGenBuffers(1, &pBuffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t_particle) * numParticles, (const GLvoid *)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_particle), (const GLvoid *)16);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cl_int result = CL_SUCCESS;
	clmem = clCreateFromGLBuffer(cl.ctx, CL_MEM_READ_WRITE, pBuffer.vbo, &result);
	CLContext::CheckCLResult(result, "ParticleSystem::createParticleBuffer");
}

void ParticleSystem::CreateGravityPointBuffer()
{
	glGenVertexArrays(1, &gpBuffer.vao);
	glBindVertexArray(gpBuffer.vao);
	glGenBuffers(1, &gpBuffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gpBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_float4) * MAX_GP, (const GLvoid *)0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cl_float4), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cl_int result = CL_SUCCESS;
	clmemgp = clCreateFromGLBuffer(cl.ctx, CL_MEM_READ_WRITE, gpBuffer.vbo, &result);
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
		this->gravityPoints.push_back(f);
	}
}

void ParticleSystem::Reset()
{
	InitParticles("init_particles_cube");
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
		{sizeof(cl_mem), &clmem},
		{sizeof(cl_mem), &clmemgp},
		{sizeof(int), &numGp},
		{sizeof(cl_float4), &clm},
		{sizeof(GLfloat), &deltaTime},
		{sizeof(int), &mouseGravity}};
	kernel->SetArgs(args, 6);
	glFinish();
	cl.AquireGLObject(clmem);
	kernel->Enqueue(cl.queue, numParticles);
	cl.ReleaseGLObject(clmem);
	CLContext::CheckCLResult(clFinish(cl.queue), "clFinish");
}

void ParticleSystem::Run()
{
	lastTime = glfwGetTime();
	double lastUpdateFpsTime = lastTime;
	int frameCount = 0;

	camera = Camera(45.0f, (float)gl.width / (float)gl.height);
	camera.position = glm::vec3(0.0, 0.0, 1.0);

	auto basicShader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	auto plane = GLObject::Plane();
	auto gp = GLObject::Triangle();

	auto renderer = GLRenderer();
	

	// auto entity = new glengine::Entity(s, quad);
	// entity->position = glm::vec3(0.0);
	// entity->rotation = glm::vec3(0.0);
	// entity->scale = glm::vec3(0.05);
	// entity->color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// auto particlePlane = new glengine::Entity(s, quad);
	// particlePlane->position = glm::vec3(0.0);
	// particlePlane->rotation = glm::vec3(0.0);
	// particlePlane->scale = glm::vec3(1.0);

	// debug axis
	float adata[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};
	GLuint avao;
	GLuint avbo;
	glGenVertexArrays(1, &avao);
	glBindVertexArray(avao);
	glGenBuffers(1, &avbo);
	glBindBuffer(GL_ARRAY_BUFFER, avbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 6, &adata[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GUIContext gui;
	gui.Init(gl.window, gl.glslVersion);

	glfwSetWindowUserPointer(gl.window, this);

	while (!glfwWindowShouldClose(gl.window) && glfwGetKey(gl.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (currentTime - lastUpdateFpsTime > 1.0)
		{
			fps = frameCount / lastUpdateFpsTime;
			lastUpdateFpsTime = currentTime;
		}

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			double xpos, ypos;
			glfwGetCursorPos(gl.window, &xpos, &ypos);
			mouseInfo.screen = glm::vec3(xpos, ypos, 0.0);
			mouseInfo.ndc = glm::vec3((float)xpos / ((float)gl.width * 0.5f) - 1.0f, (float)ypos / ((float)gl.height * 0.5f) - 1.0f, 0.0);
			mouseInfo.world = gl.GetMouseWorldCoord(&this->camera);
		}

		this->UpdateParticles(deltaTime);
		gui.Update(*this);

		// Render here!
		renderer.Begin(camera);
		glClearColor(gl.clearColor.x, gl.clearColor.y, gl.clearColor.z, gl.clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		particleShader->use();
		particleShader->setVec3("m_pos", mouseInfo.world);
		particleShader->setVec4("min_color", minColor);
		particleShader->setVec4("max_color", maxColor);
		particleShader->setMat4("proj_matrix", camera.getProjectionMatrix());
		particleShader->setMat4("view_matrix", camera.getViewMatrix());
		particleShader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), rotation, glm::vec3(1.0)));

		glPointSize(particleSize);
		glBindVertexArray(pBuffer.vao);
		glBindBuffer(GL_ARRAY_BUFFER, pBuffer.vbo);
		glDrawArrays(GL_POINTS, 0, numParticles);

		if (renderGravityPoints && gravityPoints.size() > 0)
		{
			// glPointSize(10.0f);
			// basicShader->use();
			// basicShader->setVec4("obj_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
			// basicShader->setMat4("proj_matrix", camera.getProjectionMatrix());
			// basicShader->setMat4("view_matrix", camera.getViewMatrix());
			// basicShader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), rotation, glm::vec3(1.0)));
			// glBindVertexArray(gpBuffer.vao);
			// glBindBuffer(GL_ARRAY_BUFFER, gpBuffer.vbo);
			// glDrawArrays(GL_POINTS, 0, gravityPoints.size());

			for (auto a : gravityPoints)
			{
				gp.position = glm::vec3(a.s[0], a.s[1], a.s[2]);
				gp.scale = glm::vec3(0.05);
				renderer.Draw(gp, *basicShader);
			}

		}

		// plane.position = ps->mouseInfo.world;
		// entity->position = ps->mouseInfo.world;
		// s->use();
		// s->setVec4("obj_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
		// s->setMat4("proj_matrix", camera->getProjectionMatrix());
		// s->setMat4("view_matrix", camera->getViewMatrix());
		// s->setMat4("model_matrix", entity->getModelMatrix());
		// entity->draw();

		plane.position = mouseInfo.world;
		basicShader->use();
		basicShader->setVec4("obj_color", glm::vec4(0.2, 0.3, 0.6, 0.5));
		basicShader->setMat4("proj_matrix", camera.getProjectionMatrix());
		basicShader->setMat4("view_matrix", camera.getViewMatrix());
		basicShader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), rotation, glm::vec3(1.0)));
		renderer.Draw(plane);


		// s->setVec4("obj_color", glm::vec4(0.2, 0.3, 0.6, 0.5));
		// s->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), ps->rotation, glm::vec3(1.0)));
		// particlePlane->draw();

		// s->use();
		// s->setVec4("obj_color", glm::vec4(0.0, 1.0, 0.0, 1.0));
		// s->setMat4("proj_matrix", camera->getProjectionMatrix());
		// s->setMat4("view_matrix", camera->getViewMatrix());
		// s->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0), ps->rotation, glm::vec3(1.0)));
		// glBindVertexArray(avao);
		// glBindBuffer(GL_ARRAY_BUFFER, avbo);
		// glDrawArrays(GL_LINES, 0, 3 * 6);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);


		gui.Render();

		glfwSwapBuffers(gl.window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
	gui.Shutdown();
}

