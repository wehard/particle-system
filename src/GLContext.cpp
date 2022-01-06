#include "GLContext.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "ParticleSystem.h"
#include "entity.h"
#include "mesh.h"
#include "GUIContext.h"

static void glfwMouseCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	auto ps = (ParticleSystem*)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		ps->AddGravityPoint();
	}
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	auto ps = (ParticleSystem *)glfwGetWindowUserPointer(window);
	ps->glCtx.camera->position.z += yoffset * 0.1f;
	printf("scroll x %f, y %f ", xoffset, yoffset);
	printf("camera z %f\n", ps->glCtx.camera->position.z);
}

static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

GLContext::GLContext(std::string title, int width, int height) : width(width), height(height)
{
	if (!glfwInit())
	{
		std::cout << "GLFW failed to initialize!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	this->glslVersion = "#version 150";

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cout << "GLFW failed to create window!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	camera = new glengine::Camera(45.0f, (float)width / (float)height);
	camera->position = glm::vec3(0.0, 0.0, 1.0);

	glfwSetMouseButtonCallback(window, glfwMouseCallback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSwapInterval(0);
	readGLInfo();
}

void GLContext::readGLInfo()
{
	glGetIntegerv(GL_MAJOR_VERSION, &glInfo.glMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &glInfo.glMinorVersion);
	glInfo.vendor = glGetString(GL_VENDOR);
	glInfo.renderer = glGetString(GL_RENDERER);

	glInfo.shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
}

GLContext::~GLContext()
{
	glfwTerminate();
}

static glm::vec3 intersect(glm::vec3 planeP, glm::vec3 planeN, glm::vec3 rayP, glm::vec3 rayD)
{
	float d = glm::dot(planeP, -planeN);// Vector3.Dot(planeP, -planeN);
	float t = -(d + rayP.z * planeN.z + rayP.y * planeN.y + rayP.x * planeN.x) / (rayD.z * planeN.z + rayD.y * planeN.y + rayD.x * planeN.x);
	return rayP + t * rayD;
}

static glm::vec3 projectMouse(int mouseX, int mouseY, float width, float height, glm::mat4 proj, glm::mat4 view)
{
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;

	glm::vec3 ndc = glm::vec3(x, y, z);

	// homogenouse clip space
	glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0, 1.0);

	// eye space
	glm::vec4 eye = glm::inverse(proj) * clip;

	eye = glm::vec4(eye.x, eye.y, -1.0, 0.0);

	glm::vec4 temp = glm::inverse(view) * eye;
	glm::vec3 world = glm::vec3(temp.x, temp.y, temp.z);

	world = glm::normalize(world);

	return world;
}

glm::vec3 GLContext::GetMouseWorldCoord()
{
	glm::vec3 world;
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	world = intersect(glm::vec3(0.0), glm::vec3(0.0, 0.0, 1.0), camera->position, projectMouse(xpos, ypos, width, height, camera->getProjectionMatrix(), camera->getViewMatrix()));
	return world;
}

void GLContext::run(ParticleSystem *ps)
{
	lastTime = glfwGetTime();
	double lastUpdateFpsTime = lastTime;
	int frameCount = 0;

	auto s = new glengine::Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
	auto quad = glengine::Mesh::makeQuad();
	// quad->setVertexColors(glm::vec4(1.0, 1.0, 1.0, 1.0));

	auto entity = new glengine::Entity(s, quad);
	entity->position = glm::vec3(0.0);
	entity->rotation = glm::vec3(0.0);
	entity->scale = glm::vec3(0.05);
	entity->color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	auto particlePlane = new glengine::Entity(s, quad);
	particlePlane->position = glm::vec3(0.0);
	particlePlane->rotation = glm::vec3(0.0);
	particlePlane->scale = glm::vec3(1.0);

	GUIContext gui;
	gui.Init(window, glslVersion);

	glfwSetWindowUserPointer(window, ps);

	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (currentTime - lastUpdateFpsTime > 1.0)
		{
			fps = frameCount / lastUpdateFpsTime;
			lastUpdateFpsTime = currentTime;
		}
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		ps->mouseInfo.screen = glm::vec3(xpos, ypos, 0.0);

		float mouseX = (float)xpos / ((float)width * 0.5f) - 1.0f;
		float mouseY = (float)ypos / ((float)height * 0.5f) - 1.0f;

		ps->mouseInfo.ndc = glm::vec3(mouseX, mouseY, 0.0);

		glm::mat4 proj = camera->getProjectionMatrix();
		glm::mat4 view = camera->getViewMatrix();

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			ps->mouseInfo.world = GetMouseWorldCoord(); // intersect(glm::vec3(0.0), glm::vec3(0.0, 0.0, 1.0), camera->position, projectMouse(xpos, ypos, width, height, proj, view));
		}

		// Update particles
		ps->updateGp(deltaTime);

		gui.Update(*ps);

		// Render here!
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ps->shader->use();
		ps->shader->setVec3("m_pos", ps->mouseInfo.world);
		ps->shader->setVec4("min_color", ps->minColor);
		ps->shader->setVec4("max_color", ps->maxColor);
		ps->shader->setMat4("proj_matrix", camera->getProjectionMatrix());
		ps->shader->setMat4("view_matrix", camera->getViewMatrix());
		ps->shader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), ps->rotation, glm::vec3(1.0)));

		glBindVertexArray(ps->vao);
		glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
		glDrawArrays(GL_POINTS, 0, ps->numParticles);

		// entity->position = glm::unProject(glm::vec3(xpos, ypos, 0.1), camera->getViewMatrix(), camera->getProjectionMatrix(), glm::vec4(0, 0, width, height));
		entity->position = ps->mouseInfo.world;
		s->use();
		s->setVec4("obj_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
		s->setMat4("proj_matrix", camera->getProjectionMatrix());
		s->setMat4("view_matrix", camera->getViewMatrix());
		s->setMat4("model_matrix", entity->getModelMatrix());
		entity->draw();

		s->setVec4("obj_color", glm::vec4(0.2, 0.3, 0.6, 0.5));
		s->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), ps->rotation, glm::vec3(1.0)));
		particlePlane->draw();


		gui.Render();

		glfwSwapBuffers(window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
	gui.Shutdown();
}
