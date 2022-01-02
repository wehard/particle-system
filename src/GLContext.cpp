#include "GLContext.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "ParticleSystem.h"
#include "entity.h"
#include "mesh.h"

static void mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
	auto ctx = (GLContext*)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos;
		double ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		printf("mouse screen: %f, %f | %f, %f, % f\n", xpos, ypos, ctx->m_pos.x, ctx->m_pos.y, ctx->m_pos.z);
	}
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto ctx = (GLContext*)glfwGetWindowUserPointer(window);
	ctx->camera->position.z += yoffset * 0.1f;
	printf("scroll x %f, y %f ", xoffset, yoffset);
	printf("camera z %f\n", ctx->camera->position.z);
}

static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ( glm::radians(rotation.x),  glm::radians(rotation.y),  glm::radians(rotation.z));
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_MULTISAMPLE);
	// glEnable(GL_FOG);
	// glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glClearColor(0, 0, 0, 1.f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	camera = new glengine::Camera(45.0f, (float)width / (float)height);
	camera->position = glm::vec3(0.0, 0.0, 1.0);

	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scroll_callback);
	
	glfwSwapInterval(0);
}

GLContext::~GLContext() 
{
	glfwTerminate();
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
	entity->scale = glm::vec3(0.01);
	entity->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	

	glfwSetWindowUserPointer(window, this);

	while (!glfwWindowShouldClose(window )&& glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (currentTime - lastUpdateFpsTime > 1.0)
		{
			int fps = frameCount / lastUpdateFpsTime;
			glfwSetWindowTitle(window, std::to_string(fps).c_str());
			lastUpdateFpsTime = currentTime;
		}
		double xpos;
		double ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		float mouseX = (float)xpos / ((float)width * 0.5f) - 1.0f;
		float mouseY = (float)ypos / ((float)height * 0.5f) - 1.0f;

		glm::mat4 proj = camera->getProjectionMatrix();
		glm::mat4 view = camera->getViewMatrix();

		glm::mat4 invVP = glm::inverse(proj * view);
		glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
		glm::vec4 worldPos = invVP * screenPos;

		glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

		ps->m_pos = glm::vec3(worldPos.x, worldPos.y, worldPos.z);


		this->m_pos = ps->m_pos;

		// Update particles
		ps->update(deltaTime);


		// Render here!
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ps->shader->use();
		ps->shader->setVec3("m_pos", ps->m_pos);
		ps->shader->setMat4("proj_matrix", camera->getProjectionMatrix());
		ps->shader->setMat4("view_matrix", camera->getViewMatrix());
		ps->shader->setMat4("model_matrix", getModelMatrix(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0)));

		glBindVertexArray(ps->vao);
		glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
		glDrawArrays(GL_POINTS, 0, ps->numParticles);

		// entity->position = glm::unProjectNO(glm::vec3(xpos, ypos, 0.1), camera->getViewMatrix() * entity->getModelMatrix(), camera->getProjectionMatrix(), glm::vec4(0, 0, width, height));
		entity->position = worldPos;
		s->use();
		s->setMat4("proj_matrix", camera->getProjectionMatrix());
		s->setMat4("view_matrix", camera->getViewMatrix());
		s->setMat4("model_matrix", entity->getModelMatrix());
		s->setVec4("obj_color", entity->color);
		entity->draw();

		glfwSwapBuffers(window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
}
