#include "Application.h"
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

static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	auto app = (Application *)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		app->AddGravityPoint();
	}
}

static void glfwMouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);
	if (app->mouseGravity)
		app->mouseGravityMass += yoffset * 0.1f;
}

static void glfwMouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);

	float offsetX = xpos - app->mouseInfo.screen.x;
	float offsetY = ypos - app->mouseInfo.screen.y;

	glm::vec2 offset = glm::vec2(offsetX, offsetY);

	if (app->mouseMovement)
		app->camera.Rotate(offsetX, offsetY, true);

	app->mouseInfo.screen = glm::vec3(xpos, ypos, 0.0);
	app->mouseInfo.ndc = glm::vec3((float)xpos / ((float)app->gl.width * 0.5f) - 1.0f, (float)ypos / ((float)app->gl.height * 0.5f) - 1.0f, 0.0);
	app->mouseInfo.world = app->gl.GetMouseWorldCoord(&app->camera);
}

static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto app = (Application *)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		app->mouseMovement = !app->mouseMovement;
		if (app->mouseMovement)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		app->mouseGravity = !app->mouseGravity;
	}
}

Application::Application(GLContext &gl, CLContext &cl) : gl(gl), cl(cl)
{
	clProgram = new CLProgram(this->cl, "./res/kernel/particles.cl");
	basicShader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	vertexColorShader = new Shader("res/shaders/vertex_color.vert", "res/shaders/vertex_color.frag");
	billboardShader = new Shader("res/shaders/billboard.vert", "res/shaders/billboard.frag");
	particleShader = new Shader("./res/shaders/particle.vert", "./res/shaders/particle.frag");
	emitterShader = new Shader("./res/shaders/particle_emitter.vert", "./res/shaders/particle_emitter.frag");

	glfwSetMouseButtonCallback(gl.window, glfwMouseButtonCallback);
	glfwSetScrollCallback(gl.window, glfwMouseScrollCallback);
	glfwSetKeyCallback(gl.window, glfwKeyCallback);
	glfwSetCursorPosCallback(gl.window, glfwMouseCallback);

	particleSystems.push_back(new ParticleSystem(gl, cl, *clProgram));
	particleSystems.push_back(new ParticleSystem(gl, cl, *clProgram));
}

Application::~Application()
{
	delete particleShader;
	delete emitterShader;
	delete basicShader;
	delete vertexColorShader;
	delete billboardShader;

	for (auto ps : particleSystems)
	{
		delete ps;
	}
}

void Application::Run()
{
	camera.Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);

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

		for (auto ps : particleSystems)
		{
			ps->Update(deltaTime, gravityPoints, mouseInfo);
		}
		gui.Update(*this);

		// Render here!
		renderer.Begin(camera, gl.clearColor);

		for (auto ps : particleSystems)
		{
			ps->Render(camera, mouseInfo);
		}
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
				renderer.DrawBillboard(gp, 0.05 + (mouseGravityMass * 0.001), *billboardShader);
			}

			renderer.DrawLines(wAxis, *vertexColorShader);

			// if (useEmitter)
			// {
			// 	eAxis.position = emitter.position;
			// 	eAxis.rotation = emitter.direction;
			// 	renderer.DrawLines(eAxis, *vertexColorShader);
			// }

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



void Application::AddGravityPoint()
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

void Application::AddGravityPoint(glm::vec4 pos)
{
	if (this->gravityPoints.size() < MAX_GP)
	{
		cl_float4 f;
		f.s[0] = pos.x;
		f.s[1] = pos.y;
		f.s[2] = pos.z;
		f.s[3] = pos.w;
		this->gravityPoints.push_back(f);
	}
}

void Application::ClearGravityPoints()
{
	this->gravityPoints.clear();
}

size_t Application::GetNumParticles()
{
	size_t n = 0;
	for (auto ps : particleSystems)
	{
		n += ps->numParticles;
	}
	return n;
}